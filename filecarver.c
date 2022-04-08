#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


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

	FILE *ftemp;

	unsigned char size_bmp[4];

	unsigned char cname[20];

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

	char hostbuffer[256];
    int hostname;

    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
	printf ("Extracted the files with log in name %s on host %s\n", getlogin(), hostbuffer);

	int l;

	printf("\nBMP CARVING\n");

	//fwrite(&bmp_header, sizeof(bmp_header), 1, writer1);

	int counter=0;
	while(!feof(ptr)){
		l=0;
		fread (&cur, 1, 1, ptr);
		start1++;
		if(cur==bmp_header[l]){
			l++;
			fread (&cur, 1, 1, ptr);
			start1++;
			if(cur==bmp_header[l]){
				l++;
				if(l==2){
					//printf("%s\n", "bmp_header found");
					//break;
				}

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

				bmp_size=bytes_to_decimal(size_bmp);
				
				//Setting a limit to file size 
				if(cur==test && bmp_size<300000){
					printf("%s", "bmp_header found at : ");
					printf("%lld\n", start1-6);
					printf("size in big endian : ");

					for(int i=0;i<4;i++){
						printf("%x", size_bmp[i]);
					}

					printf("\nbmp size : %d\n", bmp_size);

					sprintf(cname, "anibs2171_%d.BMP", counter);
                	ftemp = fopen(cname, "wb");
                	fwrite(&bmp_header, sizeof(bmp_header), 1, ftemp);
					

					start1=start1-4;
					int count=0;
					fseek(ptr, start1, SEEK_SET);
					while(count<bmp_size+4){
						start1++;
						fread (&cur, 1, 1, ptr);
						fwrite(&cur, sizeof(cur), 1, ftemp);
						count++;
					}
					fclose(ftemp);
					counter++;
					bmp_size=0;
					//break;
				}
			}
			l=0;
		}
		else{
			l=0;
		}	

	}

	printf("\nJPEG CARVING\n");

	counter=0;

	int j;

	while(!feof(ptr1) && counter<=4){
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
						int k;

						sprintf(cname, "anibs2171_%d.JPG", counter);
                		ftemp = fopen(cname, "wb");

						fwrite(&jpeg_header, sizeof(jpeg_header), 1, ftemp);	

						printf("%s\n", "JPEG HEX DATA");

						while(!feof(ptr1)){
							k=0;
							start++;
							fread (&cur, 1, 1, ptr1);
							fwrite(&cur, sizeof(cur), 1, ftemp);
							if(cur==jpeg_trailer[k]){
								k++;
								start++;
								fread (&cur, 1, 1, ptr1);
								fwrite(&cur, sizeof(cur), 1, ftemp);
								if(cur==jpeg_trailer[k]){
									printf("%x ---> end of trailer \n", cur);
									k++;
									counter++;
									if(k==2){
										printf("%s", "jpeg_trailer found at : ");
										printf("%lld\n", start);
										fclose(ftemp);
										break;
									}
								}
								k=0;
							}
							else{
								k=0;
							}
						}
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
	
	fclose(ptr);
	fclose(ptr1);
	return 0;
}
