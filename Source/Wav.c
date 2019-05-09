#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

struct riff_chunk {          // 'R', 'I', 'F', 'F'
	char chunk_id[4];
	uint32_t chunk_size;
	char format[4];      // 'W', 'A', 'V', 'E'
} __attribute__((packed));

struct fmt_ext_chunk {
	uint16_t chunk_size;
	int16_t valid_bits_per_sample;
	int32_t channel_mask;
	int16_t sub_format;
	char ext[];
}__attribute__((packed));

struct fmt_chunk {
	char chunk_id[4];
	uint32_t chunk_size;
	int16_t audio_format;
	int16_t num_channels;
	int32_t sample_rate;
	int32_t byte_rate;
	int16_t block_size;  // => num_channels * bits_per_sample / 8
	int16_t bits_per_sample;
	struct fmt_ext_chunk fmt_ext[];
}__attribute__((packed));


struct fact_chunk {
	char chunk_id[4];
	uint32_t chunk_size;
	int32_t sample_length;
}__attribute__((packed));

struct data_chunk {
	char chunk_id[4];
	uint32_t chunk_size;
	char data[];
}__attribute__((packed));

struct unknown_chunk {
	char chunk_id[4];
	uint32_t chunk_size;
        char unknown[];
}__attribute__((packed));

struct wav {
	struct riff_chunk *riff;
	struct fmt_chunk *fmt;
	struct fact_chunk *fact;
	struct data_chunk *data;
}__attribute__((packed));

int check_magic(struct riff_chunk *r);

int is_pcm(int16_t fmt_audio_format)
{
	if (fmt_audio_format == 1) {
                // pcm编码也有可能包含cb_size字段
		return 1;
	} else {
		// 含cb_size字段, 并且含有fact节(不绝对)
		return 0;
	}
}

struct wav *decode(const char *cur, uint32_t size, struct wav *wav)
{
	const char *end = cur+size;
	char chunk_id[4] = {0};

        // riff chunk
	wav->riff = (struct riff_chunk *)cur;
        if (check_magic(wav->riff)) {
                return (struct wav *)-1;
	}
	cur += sizeof (struct riff_chunk);

	while (cur < end) {
		if (!strncmp(cur, "fmt ", 4)) {
			wav->fmt  = (struct fmt_chunk *)cur;
			cur += wav->fmt->chunk_size + 8;
		} else if (!strncmp(cur, "fact", 4)) {
			wav->fact  = (struct fact_chunk *)cur;
			cur += wav->fact->chunk_size + 8;
		} else if  (!strncmp(cur, "data", 4)) {
			wav->data = (struct data_chunk *)cur;
			cur += wav->data->chunk_size + 8;
		} else {
			// printf("unknown chunk: %c%c%c%c\n",
			//  ((struct unknown_chunk *)cur)->chunk_id[0],
			//  ((struct unknown_chunk *)cur)->chunk_id[1],
			//  ((struct unknown_chunk *)cur)->chunk_id[2],
			//  ((struct unknown_chunk *)cur)->chunk_id[3]);

			cur += ((struct unknown_chunk *)cur)->chunk_size + 8;
		}
	}

	return wav;
}

int has_fmt_ext(struct wav *wav)
{
	return wav->fmt->chunk_size >= 18;
}

char *read_file(const char *filename)
{

	FILE *f = fopen(filename, "rb");
	if (!f) {
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = (char*)malloc(len+sizeof (int));
        buf += sizeof (int);
        ((int *)buf)[-1] = len;
	fread(buf, 1, len, f);
	fclose(f); 
	return buf;
}

int clean(char *buf) {
        buf -= sizeof (int);
	free(buf);
	return 0;
}

int buf_size(char *buf)
{
	return ((int *)buf)[-1];
}

// int encoding_map(int16_t audio_format, char *buf)
int encoding_map(struct fmt_chunk *fmt, char *buf)
{
	int16_t audio_format = fmt->audio_format;
	bzero(buf, 32);
L:
	switch (audio_format) {
		case 1:
			snprintf(buf, 32, "pcm(1)");
			break;
		case 2:
			snprintf(buf, 32, "ms adpcm(2)");
			break;
		case 3:
			snprintf(buf, 32, "IEEE float(3)");
			break;
		case 6:
			snprintf(buf, 32, "a-law(6)");
			break;
		case 7:
			snprintf(buf, 32, "mu-law(7)");
			break;
		case 49:
			snprintf(buf, 32, "gsm(49)");
			break;
		case 64:
			snprintf(buf, 32, "g.721 adpcm(64)");
			break;
		case (int16_t)-2:
			audio_format = fmt->fmt_ext->sub_format;
			if (audio_format != (int16_t)-2) {
				goto L;
			}
		default:
			snprintf(buf, 32, "%d", audio_format);
			break;
	}
	return 0;
}

int check_magic(struct riff_chunk *r)
{
        if (strncmp(r->chunk_id, "RIFF", 4)) {
                return -1;
        }
        if (strncmp(r->format, "WAVE", 4)) {
                return -1;
        }
        return 0;
}

int main(int argc, char **argv)
{
	char *buf;
	char encoding[32];
	struct wav wav = {};

        if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
                return 1;
        }
	if (!(buf = read_file(argv[1]))) {
		printf("Open file error \n", argv[0]);
		return 1;
	}

        if ((intptr_t)decode(buf, buf_size(buf), &wav)<0) {
                printf("FIAL formats - WAVE: "
				"RIFF header not found\n");
                return 2;
	}
        printf("File size: %d\n", buf_size(buf));

	printf("Rate: %d\n", wav.fmt->sample_rate);
	printf("Bits: %d\n", wav.fmt->bits_per_sample);
	printf("Channels: %d\n", wav.fmt->num_channels);

        if (has_fmt_ext(&wav)) {
		printf("cbSize: %d\n", wav.fmt->fmt_ext->chunk_size);
	}

	encoding_map(wav.fmt, encoding);
	printf("Encoding: %s\n", encoding);
	printf("Data size: %d\n", wav.data->chunk_size);

	clean(buf);
}

