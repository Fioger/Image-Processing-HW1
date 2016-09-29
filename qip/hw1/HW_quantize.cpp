// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
// Written by: ADD YOUR NAMES HERE, 2016
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	int scale = MXGRAY / levels;
	int bias = scale / 2;
	int sign = 1;
	int noise = 0;

	// init lookup table
	int i, j, lut[MXGRAY];
	for(int i=0; i<MXGRAY; ++i){
		lut[i] = scale * (i/scale) + bias;
	}
	
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	if(dither){
		int tmp;
		// evaluate output for dither: each input pixel indexes into lut[] to eval output		
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for (i=0; i< h; ++i){
				sign *= -1;
				for (j=0; j<w; ++j){
					sign *= -1;
					noise = sign * bias * ((double) rand() / RAND_MAX);
					tmp = *p1++ + noise;
					tmp = CLIP(tmp, 0, 255);
					*p2++ = lut[tmp];
				}
			}
				
		}
	} else{
	// evaluate output: each input pixel indexes into lut[] to eval output		
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for (endd = p1 + total; p1 < endd;) *p2++ = lut[*p1++];
		}
	}
}
