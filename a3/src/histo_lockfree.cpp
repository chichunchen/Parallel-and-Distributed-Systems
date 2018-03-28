#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cassert>
#include "Timer.h"

#include <thread>
#include <atomic>
#include <vector>

extern "C" {
#include "ppmb_io.h"
}

int threads;

struct img {
	int xsize;
	int ysize;
	int maxrgb;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
};

void print_histogram(FILE *f, std::array<std::atomic<int>, 256> &hist, int N) {
	fprintf(f, "%d\n", N+1);
	for(int i = 0; i <= N; i++) {
		fprintf(f, "%d %d\n", i, hist[i].load());
	}
}

// we assume hist_r, hist_g, hist_b are zeroed on entry.
void histogram(struct img *input, std::array<std::atomic<int>, 256> &hist_r, std::array<std::atomic<int>, 256> &hist_g, std::array<std::atomic<int>, 256> &hist_b, int tid) {
	int bound = input->xsize * input->ysize;
	int split = bound / threads;
	int start = tid * split;
	int end = start + split;
	if (tid+1 == threads) end = bound;

 	for(int pix = start; pix < end; pix++) {
 		hist_r[input->r[pix]]++;
 		hist_g[input->g[pix]]++;
 		hist_b[input->b[pix]]++;
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

		std::array<std::atomic<int>, 256> hist_r;
		for(auto&x: hist_r)
			  std::atomic_init(&x, 0);
		std::array<std::atomic<int>, 256> hist_g;
		for(auto&x: hist_g)
			  std::atomic_init(&x, 0);
		std::array<std::atomic<int>, 256> hist_b;
		for(auto&x: hist_b)
			  std::atomic_init(&x, 0);

		ggc::Timer t("histogram");

		t.start();

		std::thread thread_arr[threads];
 		for (int i = 0; i < threads; ++i) {
 			thread_arr[i] = std::thread(histogram, &input, std::ref(hist_r), std::ref(hist_g), std::ref(hist_b), i);
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
