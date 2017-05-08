#ifndef IMAGE_TOOLS_H_
#define IMAGE_TOOLS_H_
#include "common.h"

namespace ImageTools
{
	class CImageTools {
	public: 
		static Rect GetDrawingROI(const PointSeti& _points); 
		static Mat TranslateImage( const Mat& _img, PointF _p);
		static Mat ScaleImage(const Mat& _img, float _scale); 
		static Mat RandomImage(const Mat& _img); 
		static Mat Float2Unit(const Mat& _img);
		static bool IsValidROI(Rect _roi, Size _imgSz);
		static Mat IncreaseContrast(const Mat& _img); 
		static Rect RecfityROI(Rect _roi, Size _imgSz);
        static Rect Mask2ROI(const Mat& _mask);
        static PointF ROICenter(Rect roi);
        static bool IsValidPoint(PointF _p, Size _imgSz);
	};
}
#endif //IMAGE_TOOLS_H_
