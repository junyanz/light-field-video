#include "utils/FastImage.h"

cv::Mat FAST::CFastImage::FastAddImages( const ImageSet& _imgs ) {
	if (_imgs.empty())
		DEBUG_ERROR("NO IMAGES");
	int nImgs = (int)_imgs.size(); 
	int middle1 = ROUND(nImgs/2); 
	Mat outImg, outImg2; 
	
	#pragma omp parallel sections 
	{
		#pragma omp section 
		{
			outImg = Mat::zeros(_imgs[0].size(), _imgs[0].type());
			FOR (i, middle1)
				cv::add(outImg, _imgs[i], outImg); 
		}
		#pragma omp section 
		{
			outImg2 = Mat::zeros(_imgs[0].size(), _imgs[0].type());
			for (int i = middle1; i < nImgs; i++)
				cv::add(outImg2, _imgs[i], outImg2); 
		}
	}

	add(outImg, outImg2, outImg); 
	return outImg; 
}

float FAST::CFastImage::FastSum( Mat& _img, int _pVec[], int _n ) {
	size_t elemSize = _img.elemSize(); 
	float sum = 0.0f; 
	FOR (i, _n)
		sum += (float)(*(uchar*)(_img.data+_pVec[i]*elemSize)); 
	return sum; 
}
