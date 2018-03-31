#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cassert>
#include "Timer.h"

#include <thread>
#include <mutex>

int threads;

std::mutex r_mutex;
std::mutex g_mutex;
std::mutex b_mutex;

extern "C" {
#include "ppmb_io.h"
}

struct img {
	int xsize;
	int ysize;
	int maxrgb;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
};

void print_histogram(FILE *f, int *hist, int N) {
	fprintf(f, "%d\n", N+1);
	for(int i = 0; i <= N; i++) {
		fprintf(f, "%d %d\n", i, hist[i]);
	}
}

// we assume hist_r, hist_g, hist_b are zeroed on entry.
void histogram(struct img *input, int *hist_r, int *hist_g, int *hist_b, int tid) {
	int bound = input->xsize * input->ysize;
	int split = bound / threads;
	int start = tid * split;
	int end = start + split;
	if (tid+1 == threads) end = bound;

	for(int pix = start; pix < end; pix++) {
		r_mutex.lock();
		hist_r[input->r[pix]] += 1;
		r_mutex.unlock();

		g_mutex.lock();
		hist_g[input->g[pix]] += 1;
		g_mutex.unlock();

		b_mutex.lock();
		hist_b[input->b[pix]] += 1;
		b_mutex.unlock();
	}
}

int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("Usage: %s input-file output-file threads\n", argv[0]);
		printf("       For single-threaded runs, pass threads = 1\n");
		exit(1);
	}

	char *output_file = argv[2];
	char *input_file = argv[1];
	threads = atoi(argv[3]);

	struct img input;

	if(!ppmb_read(input_file, &input.xsize, &input.ysize, &input.maxrgb, 
				&input.r, &input.g, &input.b)) {
		if(input.maxrgb > 255) {
			printf("Maxrgb %d not supported\n", input.maxrgb);
			exit(1);
		}

		int *hist_r, *hist_g, *hist_b;
		int total_rgb = input.maxrgb + 1;

		hist_r = (int *) calloc(total_rgb, sizeof(int));
		hist_g = (int *) calloc(total_rgb, sizeof(int));
		hist_b = (int *) calloc(total_rgb, sizeof(int));

		std::thread thread_arr[threads];

		ggc::Timer t("histo_lock1");

		t.start();
		for (int i = 0; i < threads; ++i) {
			thread_arr[i] = std::thread(histogram, &input, hist_r, hist_g,
					hist_b, i);
		}
		for (int i = 0; i < threads; ++i) {
			thread_arr[i].join();
		}
		t.stop();

		FILE *out = fopen(output_file, "w");
		if(out) {
			print_histogram(out, hist_r, input.maxrgb);
			print_histogram(out, hist_g, input.maxrgb);
			print_histogram(out, hist_b, input.maxrgb);
			fclose(out);
		} else {
			fprintf(stderr, "Unable to output!\n");
		}
		printf("Time: %llu ns\n", t.duration());
	}  
}
