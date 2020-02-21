// start drawing sprites, set virtual screen space for sprites
void video_sprite_begin(
	U32 sx=640,U32 sy=480,	// intended resolution of screen (virtual screen space for video_sprite_draw)
	U32 flags=0, // see d2_viewport.h, use VP_CLEARBG if no 3d called (no setviewports)
	C32 backcolor=C32BLACK); // if flags has VP_CLEARBG

// choose between normal and font sprites (different shaders, tex/texadd) // software uses SMAT_FONT
void video_sprite_setaddmode(bool yes); // normally mul // draw a sprite with add (good for some fonts like med,beat,huge

// 'standard' uvs
#define SPT_U0    0.0001f
#define SPT_V0    0.0001f
#define SPT_U1 (1-0.0001f)
#define SPT_V1 (1-0.0001f)

// draw a quad (normal or font, depending on addmode)
void video_sprite_draw(textureb* sptex,const pointf3& col,float x0,float y0, // texture, position
	float xs=0,float ys=0,	// size, (0,0 use original texture size)
	float handx=0,float handy=0, // handle of sprite 0,0 tp sizex,sizey
	float rot=0, // rotation (CCW)
	float u0=SPT_U0,float v0=SPT_V0, // start uvs
	float u1=SPT_U1,float v1=SPT_V1); // end uvs

// draw a single triangle sprite (for curved energy/speedo meters)
void video_tri_draw(textureb* sptex,const pointf3& col,const pointf2* xys,const uv* uvs); 

// done drawing sprites, restore renderstate
void video_sprite_end(); 

