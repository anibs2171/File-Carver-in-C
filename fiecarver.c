#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

u_int32_t bytes_to_decimal(unsigned char *bytes)
{
	u_int32_t val = 0;
	for (int i = 0; i < 4; i++)
	{
		val <<= 8;
		val |= *bytes++;
	}
	return val;
}

int main(){
	FILE *ptr;
	ptr=fopen("samples", "rb");

	FILE *ptr1;
	ptr1=fopen("samples", "rb");
	unsigned char jpeg_header[3] = {0xFF, 0xD8, 0xFF } ;
	unsigned char jpeg_trailer[2] = { 0xFF, 0xD9 } ;
	unsigned char bmp_header[2] = { 0x42, 0x4D } ;

	unsigned char test={0x00};

	FILE *writer = fopen("output.JPG","wb");
	FILE *writer1 = fopen("output.BMP","wb");

	unsigned char size_bmp[4];

	unsigned char cur;
	long long int start1=0;
	long long int start=0;
	int bmp_size;
	if(ptr == NULL){
		printf("file %s cant be opened\n", "samples");
		exit(0);
	}
	else{
		printf("file %s opened\n", "samples");
	}

	int l;

	printf("\nBMP CARVING\n");

	fwrite(&bmp_header, sizeof(bmp_header), 1, writer1);

	while(!feof(ptr)){
		l=0;
		fread (&cur, 1, 1, ptr);
		start1++;
		if(cur==bmp_header[l]){
			l++;
			fread (&cur, 1, 1, ptr);
			start1++;
			//as files start at a sector beginning
			if(cur==bmp_header[l] && (start1-2)%512==0){
				l++;
				if(l==2){
					//printf("%s\n", "bmp_header found");
					//break;
				}

				FILE *startbmp=ptr;

				fread(&cur, 1, 1, ptr);
				start1++;
				size_bmp[3]=cur;
				fread(&cur, 1, 1, ptr);	
				start1++;
				size_bmp[2]=cur;
				fread(&cur, 1, 1, ptr);
				start1++;
				size_bmp[1]=cur;
				fread(&cur, 1, 1, ptr);
				start1++;
				size_bmp[0]=cur;

				//first smaller bmp file ---> i.e which has 00 in the last byte in little endian

				if(cur==test){
					printf("%s", "bmp_header found at : ");
					printf("%lld\n", start1-6);
					printf("size in big endian : ");

					for(int i=0;i<4;i++){
						printf("%x", size_bmp[i]);
					}

					bmp_size=bytes_to_decimal(size_bmp);

					printf("\nbmp size : %d\n", bmp_size);

					int count=0;
					fseek(ptr, start1-4, SEEK_SET);
					while(count<bmp_size+4){
						fread (&cur, 1, 1, ptr);
						fwrite(&cur, sizeof(cur), 1, writer1);
						count++;
					}

					break;
				}

					
				
			}
			l=0;
		}
		else{
			l=0;
		}	

	}

	printf("\nJPEG CARVING\n");

	int j;

	while(!feof(ptr1)){
		j=0;
		start++;
		fread (&cur, 1, 1, ptr1);
		if(cur==jpeg_header[j] && j==0){
			j++;
			start++;
			fread (&cur, 1, 1, ptr1);
			if(cur==jpeg_header[j] && j==1){
				j++;
				start++;
				fread (&cur, 1, 1, ptr1);
				if(cur==jpeg_header[j] && j==2){
					j++;
					printf("%x ---> end of header \n", cur);
					if(j==3){
						printf("%s", "jpeg_header found at : ");
						printf("%lld\n", start);
						break;
					}
				}
				j=0;
			}
			j=0;
		}
		
		
		else{
			j=0;
		}
	}

	int k;

	fwrite(&jpeg_header, sizeof(jpeg_header), 1, writer);	

	printf("%s\n", "JPEG HEX DATA");

	while(!feof(ptr1)){
		k=0;
		start++;
		fread (&cur, 1, 1, ptr1);
		fwrite(&cur, sizeof(cur), 1, writer);
		if(cur==jpeg_trailer[k]){
			k++;
			start++;
			fread (&cur, 1, 1, ptr1);
			fwrite(&cur, sizeof(cur), 1, writer);
			if(cur==jpeg_trailer[k]){
				printf("%x ---> end of trailer \n", cur);
				k++;
				if(k==2){
					printf("%s", "jpeg_trailer found at : ");
					printf("%lld\n", start);
					break;
				}
			}
			k=0;
		}
		else{
			k=0;
		}
	}



	return 0;
}
