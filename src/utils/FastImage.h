#ifndef FAST_IMAGE_H_
#define FAST_IMAGE_H_
#include "common.h"
namespace FAST {
	class CFastImage {
	public: 
		CFastImage() {};
		~CFastImage() {};

		static void Mask2Index(const Mat& _mask, int _pVec[], int& _nPixels) ; 
		static float FastSSD(Mat _img1, Mat _img2, const int _pVec[], int _nPixels);
		static float FastSSD(Mat _img1, Mat _img2, const int _pVec1[],  const int _pVec2[], int _nPixels);
		static float FastConvolution(Mat _img1, Mat _img2, const int _pVec[], int _nPixels);
		static float FastSSDCentral(Mat _img1, Mat _img2, Mat _weight, float _norm, const int _pVec[], int _nPixel);
		static float FastSSD(Mat _img, uchar _p[], const int _pVec[], int _nPixel); 
		static void FastErrorImage(Mat _img1, Mat _img2, Mat& _out, const int _pVec[], int _nPixel); 
		static float FastConvolution(Mat _img, const int _pVec[], int _nPixels);
		//static float FastConvolution(Mat _img1, )
		static void NormalizePatch(Mat& _img); 
		static void MagnifyPatch(Mat& _img, int _factor); 
		static void ShowImages(vector<Mat> _imgVec, string _name, 
			int _hGrid, int _wGrid, int _scale); 
		static void ThresholdZeroError(vector<float>& _errVec); 
		static void UpdateMask(Size _imgSz, const int _pVec[], int _nPixel, 
			int _offset_x, int _offset_y, int _outPVec[], int& _outPixel); 
		static Mat FastAddImages(const vector<Mat>& _imgs); 
		static float FastSum(Mat& _img, int _pVec[], int _n); 
	};
}
#endif //FAST_IMAGE_H_