#include <stdio.h>
#include <stdint.h>

// compute basis functions hh[] is {p0,m0,p1,m1}

static void compute_hh(float t, float hh[4]) {
	float t2=t*t;
	float t3=t2*t;
	hh[0]=2*t3-3*t2+1;
	hh[1]=t3-2*t2+t;
	hh[2]=-2*t3+3*t2;
	hh[3]=t3-t2;
}


// d[8] is {x0,y0,d0, x1,y1,d1}

float hermite(float x,float d[6]) {
	float tt=d[3]-d[0];
	float t=(x-d[0])/tt;

	float hh[4];
	compute_hh(t,hh);

	return hh[0]*d[1] + tt*hh[1]*d[2] + hh[2]*d[4] + tt*hh[3]*d[5];
}


// d[8] is {x0,y0,x1,y1,x2,y2,x3,y3}
// r[2] is {x,y}

void bezier(float t,float d[8],float r[2]) {
	float q=t*t;
	float c=t*q;
	float n=1-t;
	float Q=n*n;
	float C=n*Q;

	r[0]=C*d[0] + 3*t*Q*d[2] + 3*q*n*d[4] + c*d[6];
	r[1]=C*d[1] + 3*t*Q*d[3] + 3*q*n*d[5] + c*d[7];
}


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

uint8_t framebuffer[256][256][3];

void dump_as_png(void) {
       stbi_write_png("dump.png",256,256,3,framebuffer,3*256);
}


void test_hermit() {
	float d[]={0,0,0,128,255,0,NAN};
	int i;
	for(i=0;i<255;i++) {
		if(i<d[0]) continue;
		if(i>d[3]) continue;
		float r=hermite(i,d);
		if(r>=0 && r<=255) framebuffer[(int)r][i][2]=255;
		printf("%u %.02f\n",i,r);
	}
	dump_as_png();
}

float bezier_points[256];

void test_bezier() {
	float d[]={0,0,50,0, 128-70,255,128,255};
	float t=0;
	float dt=0.02;
	int x=d[0];

	for(;;) {
		float r[2];
		bezier(t,d,r);
		float dx=r[0]-x;

		if(dx>0.1) {
			if(dt>0) dt=-dt/2;
			t+=dt;
			continue;
		}
		if(dx<-0.1) {
			if(dt<0) dt=-dt/2;
			t+=dt;
			continue;
		}

		if(x==d[6]) break;
		x++;
		dt=0.02;

		if(r[0]<0 || r[0]>255) continue;
		if(r[1]<0 || r[1]>255) continue;

		framebuffer[(int)r[1]][x][0]=0xff;
		framebuffer[(int)r[1]][x][1]=0xff;
		framebuffer[(int)r[1]][x][2]=0xff;

		bezier_points[x]=r[1];
	}

	if(d[3]>=0 && d[3]<=255 && d[2]>=0 && d[2]<=255) {
		framebuffer[(int)d[3]][(int)d[2]][0]=0xff;
		framebuffer[(int)d[3]][(int)d[2]][1]=0x0;
		framebuffer[(int)d[3]][(int)d[2]][2]=0x0;
	}

	if(d[5]>=0 && d[5]<=255 && d[4]>=0 && d[4]<=255) {
		framebuffer[(int)d[5]][(int)d[4]][0]=0x0;
		framebuffer[(int)d[5]][(int)d[4]][1]=0xff;
		framebuffer[(int)d[5]][(int)d[4]][2]=0x0;
	}
	dump_as_png();
	int i;
	printf("{");
	for(i=0;i<256;i++) {
		printf("%u,",(int)bezier_points[i]);
	}
	printf("}\n");
}

int main() {
	memset(framebuffer,0,sizeof(framebuffer));
	test_bezier();
}

