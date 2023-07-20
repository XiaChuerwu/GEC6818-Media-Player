#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "image_thumbnail.h"

struct my_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */
 
    jmp_buf setjmp_buffer;	/* for return to caller */
};
 
typedef struct my_error_mgr * my_error_ptr;
 
/*
 * Here's the routine that will replace the standard error_exit method:
 */
METHODDEF(void)
    my_error_exit (j_common_ptr cinfo){
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
 
    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);
 
    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}


unsigned char* ReadJpeg(const char * pathname, int* width, int* height){
	FILE *file = fopen(pathname, "rb");
	if ( file == NULL )	{
		return NULL;
	}
 
	struct jpeg_decompress_struct info; //for our jpeg info
 
// 	struct jpeg_error_mgr err; //the error handler
// 	info.err = jpeg_std_error(&err);
 
	struct my_error_mgr my_err;

	info.err = jpeg_std_error(&my_err.pub);
	my_err.pub.error_exit = my_error_exit;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(my_err.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		printf("Error occured\n");
		jpeg_destroy_decompress(&info);
		fclose(file);
		return NULL;
	}
 
	jpeg_create_decompress( &info ); //fills info structure
	jpeg_stdio_src( &info, file );        //void
 
	int ret_Read_Head = jpeg_read_header( &info, 1 ); //int
 
	if(ret_Read_Head != JPEG_HEADER_OK){
		printf("jpeg_read_header failed\n");
		fclose(file);
		jpeg_destroy_decompress(&info);
		return NULL;
	}
 
	int bStart = jpeg_start_decompress( &info );
	if(!bStart){
		printf("jpeg_start_decompress failed\n");
		fclose(file);
		jpeg_destroy_decompress(&info);
		return NULL;
	}
	int w = *width = info.output_width;
	int h = *height = info.output_height;
	int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
	unsigned long dataSize = w * h * numChannels;
 
	// read RGB(A) scanlines one at a time into jdata[]
	unsigned char *data = (unsigned char *)malloc( dataSize );
	if(!data) return NULL;
 
	unsigned char* rowptr;
	while ( info.output_scanline < h )
	{
		rowptr = data + info.output_scanline * w * numChannels;
		jpeg_read_scanlines( &info, &rowptr, 1 );
	}

	jpeg_finish_decompress( &info );    
 
	fclose( file );
 
	return data;
}


/*参数为：
 *返回图片的宽度(w_Dest),
 *返回图片的高度(h_Dest),
 *返回图片的位深(bit_depth),
 *源图片的RGB数据(src),
 *源图片的宽度(w_Src),
 *源图片的高度(h_Src)
 */
unsigned char* do_Stretch_Linear(int w_Dest,int h_Dest,int bit_depth,unsigned char *src,int w_Src,int h_Src){
	int sw = w_Src-1, sh = h_Src-1, dw = w_Dest-1, dh = h_Dest-1;
	int B, N, x, y;
	int nPixelSize = bit_depth/8;
	unsigned char *pLinePrev,*pLineNext;
	unsigned char *pDest = (unsigned char*)malloc(w_Dest*h_Dest*bit_depth/8);
	unsigned char *tmp;
	unsigned char *pA,*pB,*pC,*pD;
 
	for(int i=0;i<=dh;++i)
	{
		tmp =pDest + i*w_Dest*nPixelSize;
		y = i*sh/dh;
		N = dh - i*sh%dh;
		pLinePrev = src + (y++)*w_Src*nPixelSize;
		//pLinePrev =(unsigned char *)aSrc->m_bitBuf+((y++)*aSrc->m_width*nPixelSize);
		pLineNext = (N==dh) ? pLinePrev : src+y*w_Src*nPixelSize;
		//pLineNext = ( N == dh ) ? pLinePrev : (unsigned char *)aSrc->m_bitBuf+(y*aSrc->m_width*nPixelSize);
		for(int j=0;j<=dw;++j)
		{
			x = j*sw/dw*nPixelSize;
			B = dw-j*sw%dw;
			pA = pLinePrev+x;
			pB = pA+nPixelSize;
			pC = pLineNext + x;
			pD = pC + nPixelSize;
			if(B == dw)
			{
				pB=pA;
				pD=pC;
			}
 
			for(int k=0;k<nPixelSize;++k)
			{
				*tmp++ = ( unsigned char )( int )(
					( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
					+ dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
					+ dw * dh / 2 ) / ( dw * dh ) );
			}
		}
	}
	return pDest;
}
 
int write_JPEG_file (const char * filename, unsigned char* image_buffer, int quality,int image_height, int image_width){
 
	if(filename == NULL || image_buffer == NULL) return -1;
 
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;		/* target file */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */
	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);
 
	if ((outfile = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "can't open %s\n", filename);
		return -1;
	}
	jpeg_stdio_dest(&cinfo, outfile);
 
	cinfo.image_width = image_width; 	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
 
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
 
	jpeg_start_compress(&cinfo, TRUE);
 
	row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */
 
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
 
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
 
	jpeg_destroy_compress(&cinfo);
 
	return 1;
}
 


/*
    功能：
        生成图片的缩略图（图片的一个缩小版本）
    参数:
        inputFile： 源文件路径名
        outputFile：输出缩略图路径名
*/
int generate_image_thumbnail(const char* inputFile, const char* outputFile){
	if(inputFile == NULL || outputFile == NULL) return -1;

	//读取jpeg图片像素数组
	int w=0,h=0;
	unsigned char* buff = ReadJpeg(inputFile,&w,&h);
	if(buff == NULL) {
		printf("ReadJpeg Failed\n");
		return -1;
	}

	//缩放图片，缩放后的大小为(tb_w,tb_h)
	int tb_w = 160, tb_h = 160;
	unsigned char * img_buf = do_Stretch_Linear(tb_w,tb_h,24,buff,w,h);
	free(buff);
 
	//将缩放后的像素数组保存到jpeg文件
	int bRetWrite = write_JPEG_file(outputFile,img_buf,100,tb_w,tb_h);
	free(img_buf);
 
	if(bRetWrite){
		return 1;
	}else{
		printf("GenerateImageThumbnail: write failed\n");
		return 1;
	}
}