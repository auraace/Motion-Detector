#include <stdio.h>
#include <omp.h>
# include <stdlib.h>
# include <math.h> 
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>  
#include "lodepng.h"

sem_t mutex;
int numt;
int pixels;
int threads;
int curt;
int count;

std::vector<unsigned char> image; //the raw pixels
std::vector<unsigned char> image2;
std::vector<unsigned char> grey1;//grey-scale of the first image
std::vector<unsigned char> grey2;//grey-scale of the second image
std::vector<unsigned char> diff;//time differece of the two images

void *child(void*);
int main (int argc, char *argv[]) {
	sem_init(&mutex, 0, 1);
	//numt = atoi(argv[1]);
	//const char* filename = "image1.png";
	//const char* filename2 = "image2.png";
	const char* filename = argv[1];
	const char* filename2 = argv[2];
	threads = atoi(argv[3]);
	curt = 0;
	count = 0;
	printf("number of threads: %d\n",threads);
	
	unsigned width, height;
	unsigned width2, height2;

	//decode
	unsigned error = lodepng::decode(image, width, height, filename);
	unsigned error2 = lodepng::decode(image2, width2, height2, filename2);
	
	pixels = (int)width * (int)height;
	printf("width: %d, height: %d, pixels: %d\n",width, height, pixels);

	int total = 0;
	double average = 0;

	int i,j;
	int p;
	int g1,g2,d;
	
	pthread_t tid; /* the thread identifier */
	pthread_attr_t attr; /* set of attributes for the thread */
	pthread_attr_init(&attr);
	for(i=0;i<threads;i++)
	{
		//printf("thread created\n");
		pthread_create(&tid,&attr,child,NULL);
	}
	for(j=0;j<threads;j++)
	{
		pthread_join(tid,NULL);
	}
	printf("the program did %d calculations\n",count);
	for(i=0;i<diff.size()/4;i++)
	{
		p = 4*i;
		total += diff.at(p);
	}
	average = (1.0 * total) / (1.0 * pixels);
	printf("total: %d, average: %.2f\n", total, average);
	if(average >= 5)
		printf("motion detected\n");
	else
		printf("no motion detected\n");
	
	const char* outfile = "time_difference.png";
	error = lodepng::encode(outfile, diff, width, height);//outputs the time difference image to time_difference.png
	
	const char* gout1 = "grey-scale1.png";
	const char* gout2 = "grey-scale2.png";
	
	error = lodepng::encode(gout1, grey1, width, height);
	error = lodepng::encode(gout2, grey2, width, height);
}
void *child(void* temp) 
{
	int tnum;
	tnum = curt;
	curt++;
	int i;
	int p;
	int g1,g2,d;
	int interval = pixels/threads;
	int x_start = tnum * interval;
	int x_stop = x_start + interval;
	printf("thread #%d starts at pixel # %d, end at # %d\n",tnum,x_start,x_stop);
	
	for(i=x_start;i<x_stop;i++)
	{
		
		p = 4*i;
		sem_wait(&mutex);//start of the critical section
		g1 = (image.at(p)+image.at(p+1)+image.at(p+2))/3;
		grey1.push_back(g1);
		grey1.push_back(g1);
		grey1.push_back(g1);
		grey1.push_back(image.at(p+3));
		g2 = (image2.at(p)+image2.at(p+1)+image2.at(p+2))/3;		
		grey2.push_back(g2);
		grey2.push_back(g2);
		grey2.push_back(g2);
		grey2.push_back(image2.at(p+3));
		if(g1 > g2)
			d = g1 - g2;
		else
			d = g2 - g1;
		//printf("%d, ",d);
		diff.push_back(d);
		diff.push_back(d);
		diff.push_back(d);
		diff.push_back(image.at(p+3));
		count++;
		sem_post(&mutex);//end of the critical section
		
	}
	pthread_exit(0);
} 