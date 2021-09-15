/* FastLED_RGBW
 * 
 * Hack to enable SK6812 RGBW strips to work with FastLED.
 *
 * Original code by Jim Bumgardner (http://krazydad.com).
 * Modified by David Madison (http://partsnotincluded.com).
 * 
*/

#ifndef FastLED_RGBW_h
#define FastLED_RGBW_h

struct CRGBW  {
	union {
		struct {
			union {
				__UINT8_TYPE__ g;
				__UINT8_TYPE__ green;
			};
			union {
				__UINT8_TYPE__ r;
				__UINT8_TYPE__ red;
			};
			union {
				__UINT8_TYPE__ b;
				__UINT8_TYPE__ blue;
			};
			union {
				__UINT8_TYPE__ w;
				__UINT8_TYPE__ white;
			};
		};
		__UINT8_TYPE__ raw[4];
	};

	CRGBW(){}

	CRGBW(__UINT8_TYPE__ rd, __UINT8_TYPE__ grn, __UINT8_TYPE__ blu, __UINT8_TYPE__ wht){
		r = rd;
		g = grn;
		b = blu;
		w = wht;
	}

	inline void operator = (const CRGB c) __attribute__((always_inline)){ 
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->white = 0;
	}
};

inline __UINT16_TYPE__ getRGBWsize(__UINT16_TYPE__ nleds){
	__UINT16_TYPE__ nbytes = nleds * 4;
	if(nbytes % 3 > 0) return nbytes / 3 + 1;
	else return nbytes / 3;
}

#endif