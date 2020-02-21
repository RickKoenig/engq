#include <m_eng.h>
static bitmap32* tbm;
static int cnt;
#if 0
int movieinit(char* fname)
{
	tbm=bitmap32alloc(100,50,C32BLACK);
	cnt=0;
	return 24;
}

bitmap32* moviegetframe()
{
	if (tbm) {
		clipclear32(tbm,C32MAGENTA);
		outtextxyf32(tbm,8,8,C32WHITE,"%d",cnt++);
	}
	return tbm;
}

void movieexit()
{
	bitmap32free(tbm);
	tbm=0;
}
#endif
#if 1
#include <stdio.h>
#include <theora/theora.h>

static struct wavehandle* wh;
//static int whplaying;
//////// movie system
//static struct tex* movietx;
static int moviefrm;

static	ogg_sync_state oy;
static ogg_page          og;
static	theora_info ti;
static 	theora_comment tc;
static ogg_stream_state  to;
static theora_state      td;
static int stateflag;
static int theora_p=0;
static FILE* infile;
static int          raw=0;
static int          videobuf_ready=0;
static ogg_int64_t  videobuf_granulepos=-1;
static int eom;
static ogg_packet op;
static int frames;
static int doplay;

/* Helper; just grab some more compressed bitstream and sync it for
   page extraction */
static int buffer_data(FILE *in,ogg_sync_state *oy){
  char *buffer=ogg_sync_buffer(oy,4096);
  int bytes=fread(buffer,1,4096,in);
  ogg_sync_wrote(oy,bytes);
  return(bytes);
}

/* this is a nop in the current implementation. we could
   open a file here or something if so moved. */
static void open_video(void){
  return;
}

static int queue_page(ogg_page *page){
  if(theora_p)ogg_stream_pagein(&to,page);
  return 0;
}

/* dump the theora comment header */
static int dump_comments(theora_comment *tc){
  int i, len;
  char *value;
//  FILE *out=stderr;

//  fprintf(out,"Encoded by %s\n",tc->vendor);
  if(tc->comments){
//    fprintf(out, "theora comment header:\n");
    for(i=0;i<tc->comments;i++){
      if(tc->user_comments[i]){
        len=tc->comment_lengths[i];
        value=(char*)malloc(len+1);
        memcpy(value,tc->user_comments[i],len);
        value[len]='\0';
//        fprintf(out, "\t%s\n", value);
        free(value);
      }
    }
  }
  return(0);
}

static void video_write(void){
//  int i;

  yuv_buffer yuv;
  theora_decode_YUVout(&td,&yuv);

/*  if(outfile){
    if(!raw)
      fprintf(outfile, "FRAME\n");
    for(i=0;i<yuv.y_height;i++)
      fwrite(yuv.y+yuv.y_stride*i, 1, yuv.y_width, outfile);
    for(i=0;i<yuv.uv_height;i++)
      fwrite(yuv.u+yuv.uv_stride*i, 1, yuv.uv_width, outfile);
    for(i=0;i<yuv.uv_height;i++)
      fwrite(yuv.v+yuv.uv_stride*i, 1, yuv.uv_width, outfile);
  } */
}

// returns fps
int movieinit(char *fname,S32& wid,S32& hit)
{
	int fps=0;
//  int long_option_index;
//  int c;

//  struct timeb start;
//  struct timeb after;
//  struct timeb last;
//  int fps_only=0;
  wh=wave_load(fname,0);
//  wh=wave_load("04 starman.ogg",0);
  doplay=1;

//	whplaying=0;
	eom = 0;

//  FILE *infile = stdin;
//  outfile = stdout;

#ifdef _WIN32 /* We need to set stdin/stdout to binary mode on windows. */
  /* Beware the evil ifdef. We avoid these where we can, but this one we
     cannot. Don't add any more, you'll probably go to hell if you do. */
//  _setmode( _fileno( stdin ), _O_BINARY );
//  _setmode( _fileno( stdout ), _O_BINARY );
#endif
  frames = 0;
	stateflag=0;
	theora_p=0;
	raw=0;
	videobuf_ready=0;

//	movietx=mt;
	moviefrm=0;

//	return;
	infile = fopen2(fname,"rb");
  /* Process option arguments. */
/*  while((c=getopt_long(argc,argv,optstring,options,&long_option_index))!=EOF){
    switch(c){
    case 'o':
      if(strcmp(optarg,"-")!=0){
        outfile=fopen(optarg,"wb");
        if(outfile==NULL){
          fprintf(stderr,"Unable to open output file '%s'\n", optarg);
          exit(1);
        }
      }else{
        outfile=stdout;
      }
      break;

    case 'r':
      raw=1;
      break;

    case 'f':
      fps_only = 1;
      outfile = NULL;
      break;

    default:
      usage();
    }
  }
  if(optind<argc){
    infile=fopen(argv[optind],"rb");
    if(infile==NULL){
      fprintf(stderr,"Unable to open '%s' for extraction.\n", argv[optind]);
      exit(1);
    }
    if(++optind<argc){
      usage();
      exit(1);
    }
  }
*/
  /*
     Ok, Ogg parsing. The idea here is we have a bitstream
     that is made up of Ogg pages. The libogg sync layer will
     find them for us. There may be pages from several logical
     streams interleaved; we find the first theora stream and
     ignore any others.

     Then we pass the pages for our stream to the libogg stream
     layer which assembles our original set of packets out of
     them. It's the packets that libtheora actually knows how
     to handle.
  */

  /* start up Ogg stream synchronization layer */
  ogg_sync_init(&oy);

  /* init supporting Vorbis structures needed in header parsing */
  /*vorbis_info_init(&vi);*/
  /*vorbis_comment_init(&vc);*/

  /* init supporting Theora structures needed in header parsing */
  theora_comment_init(&tc);
  theora_info_init(&ti);

  /* Ogg file open; parse the headers */

  /* Vorbis and Theora both depend on some initial header packets
     for decoder setup and initialization. We retrieve these first
     before entering the main decode loop. */

  /* Only interested in Theora streams */
  while(!stateflag){
    int ret=buffer_data(infile,&oy);
    if(ret==0)break;
    while(ogg_sync_pageout(&oy,&og)>0){
      ogg_stream_state test;

      /* is this a mandated initial header? If not, stop parsing */
      if(!ogg_page_bos(&og)){
        /* don't leak the page; get it into the appropriate stream */
        queue_page(&og);
        stateflag=1;
        break;
      }

      ogg_stream_init(&test,ogg_page_serialno(&og));
      ogg_stream_pagein(&test,&og);
      ogg_stream_packetout(&test,&op);

      /* identify the codec: try theora */
      if(!theora_p && theora_decode_header(&ti,&tc,&op)>=0){
			  fps = ti.fps_numerator / 1000;
//			  *fps /= 2; // adjust for timing bug by a factor of 2..
//		  }
        /* it is theora -- save this stream state */
        memcpy(&to,&test,sizeof(test));
        theora_p=1;
      }else{
        /* whatever it is, we don't care about it */
        ogg_stream_clear(&test);
      }
    }
    /* fall through to non-bos page parsing */
  }

  /* we're expecting more header packets. */
  while(theora_p && theora_p<3){
    int ret;

    /* look for further theora headers */
    while(theora_p && (theora_p<3) && (ret=ogg_stream_packetout(&to,&op))){
      if(ret<0){
        fprintf(stderr,"Error parsing Theora stream headers; "
         "corrupt stream?\n");
        exit(1);
      }
      if(theora_decode_header(&ti,&tc,&op)){
        fprintf(stderr,"Error parsing Theora stream headers; "
         "corrupt stream?\n");
        exit(1);
      }
      theora_p++;
      if(theora_p==3)break;
    }

    /* The header pages/packets will arrive before anything else we
       care about, or the stream is not obeying spec */

    if(ogg_sync_pageout(&oy,&og)>0){
      queue_page(&og); /* demux into the appropriate stream */
    }else{
      int ret=buffer_data(infile,&oy); /* someone needs more data */
      if(ret==0){
        fprintf(stderr,"End of file while searching for codec headers.\n");
        exit(1);
      }
    }
  }
  dump_comments(&tc);

  /* and now we have it all.  initialize decoders */
  if(theora_p){
    theora_decode_init(&td,&ti);
    fprintf(stderr,"Ogg logical stream %lx is Theora %dx%d %.02f fps video\n"
     "Encoded frame content is %dx%d with %dx%d offset\n",
     to.serialno,ti.width,ti.height,
     (double)ti.fps_numerator/ti.fps_denominator,
     ti.frame_width,ti.frame_height,ti.offset_x,ti.offset_y);
  }else{
    /* tear down the partial theora setup */
    theora_info_clear(&ti);
    theora_comment_clear(&tc);
  }

  /* open video */
  if(theora_p)open_video();

  if(!raw){
//    logger("YUV4MPEG2 W%d H%d F%d:%d I%c A%d:%d\n",
//     ti.width,ti.height,ti.fps_numerator,ti.fps_denominator,'p',
//     ti.aspect_numerator,ti.aspect_denominator);
  }
/*  if(!raw && outfile){
    fprintf(outfile,"YUV4MPEG2 W%d H%d F%d:%d I%c A%d:%d\n",
     ti.width,ti.height,ti.fps_numerator,ti.fps_denominator,'p',
     ti.aspect_numerator,ti.aspect_denominator);
  } */

  /* install signal handler */
//  signal (SIGINT, sigint_handler);

  /* Finally the main decode loop. 

     It's one Theora packet per frame, so this is pretty 
     straightforward if we're not trying to maintain sync
     with other multiplexed streams.

     the videobuf_ready flag is used to maintain the input
     buffer in the libogg stream state. If there's no output
     frame available at the end of the decode step, we must
     need more input data. We could simplify this by just 
     using the return code on ogg_page_packetout(), but the
     flag system extends easily to the case were you care
     about more than one multiplexed stream (like with audio
     playback). In that case, just maintain a flag for each
     decoder you care about, and pull data when any one of
     them stalls.

     videobuf_time holds the presentation time of the currently
     buffered video frame. We ignore this value.
  */

  stateflag=0; /* playback has not begun */
  /* queue any remaining pages from data we buffered but that did not
      contain headers */
  while(ogg_sync_pageout(&oy,&og)>0){
    queue_page(&og);
  }

/*  if(fps_only){
    ftime(&start);
    ftime(&last);
  } */
  wid=ti.width;
  hit=ti.height;
	return fps;
}

#define clip(a) (a<0 ? 0 : a>255 ? 255 : a)

static void colorconv(unsigned char y,unsigned char u,unsigned char v,unsigned int* c)
{
	int r = (9535 * (y - 0) + 13074 * ((int)v - 128)) >> 13;
	int g = (9535 * (y - 0) - 6660  * ((int)v - 128) - 3203 * ((int)u - 128)) >> 13;
	int b = (9535 * (y - 0) + 16531 * ((int)u - 128)) >> 13;
	r=clip(r);
	g=clip(g);
	b=clip(b);
	*c = b + (g<<8) + (r<<16);
}

bitmap32* moviegetframe()
{
//	int prod;
//	int outw,outh;
	int i,j=0;
//	D3DLOCKED_RECT lr;
//	HRESULT hr;
	yuv_buffer yuv;

	if (eom)
		return 0;
	while(1) {
	//  while(!got_sigint){

		while(theora_p && !videobuf_ready){
	/* theora is one in, one out... */
			if(ogg_stream_packetout(&to,&op)>0){

				theora_decode_packetin(&td,&op);
				videobuf_granulepos=td.granulepos;
				//        videobuf_time=theora_granule_time(&td,videobuf_granulepos);
				videobuf_ready=1;
				frames++;
	//        if(fps_only)
	//          ftime(&after);

			}else {
				break;
			}
		}

	/*    if(fps_only && (videobuf_ready || fps_only==2)){
	long ms =
	after.time*1000.+after.millitm-
	(last.time*1000.+last.millitm);

	if(ms>500 || fps_only==1 ||
	(feof(infile) && !videobuf_ready)){
	float file_fps = (float)ti.fps_numerator/ti.fps_denominator;
	fps_only=2;

	ms = after.time*1000.+after.millitm-
	(start.time*1000.+start.millitm);

	fprintf(stderr,"\rframe:%d rate:%.2fx           ",
	frames,
	frames*1000./(ms*file_fps));
	memcpy(&last,&after,sizeof(last));
	}
	}
	*/
		if(!videobuf_ready && feof(infile)) {
			eom=1;
			break;
		}

		if(!videobuf_ready){
	/* no data yet for somebody.  Grab another page */
			buffer_data(infile,&oy);
			while(ogg_sync_pageout(&oy,&og)>0){
				queue_page(&og);
			}
		}
	/* dumpvideo frame, and get new one */
	//    else if(outfile)video_write();
		else {
			video_write();
			videobuf_ready=0;
			break;
		}

	}

	/* end of decoder loop -- close everything */

	//  if(outfile && outfile!=stdout)fclose(outfile);

//	fprintf(stderr, "\n\n%d frames\n", frames);
//	fprintf(stderr, "\nDone.\n");

	if (eom)
		return 0;

//  int i;

	theora_decode_YUVout(&td,&yuv);

/*  if(outfile){
    if(!raw)
      fprintf(outfile, "FRAME\n");
    for(i=0;i<yuv.y_height;i++)
      fwrite(yuv.y+yuv.y_stride*i, 1, yuv.y_width, outfile);
    for(i=0;i<yuv.uv_height;i++)
      fwrite(yuv.u+yuv.uv_stride*i, 1, yuv.uv_width, outfile);
    for(i=0;i<yuv.uv_height;i++)
      fwrite(yuv.v+yuv.uv_stride*i, 1, yuv.uv_width, outfile);
  } */
	if (!tbm) {
//		if (0) //yuv.y_height==304)
//			tbm=bitmap32alloc(yuv.y_width,300,C32BLACK); // hack to get rid of green bar on bottom (BETA)
//		else
			tbm=bitmap32alloc(yuv.y_width,yuv.y_height,C32GREEN);
	}
//	hr=IDirect3DTexture8_LockRect(movietx->t,0,&lr,0,0);
//	if (hr!=D3D_OK)
//		errorexit("can't lock movie tex %x",hr);
	// copy movie frame into locked texture
	if (yuv.y_width==yuv.uv_width && yuv.y_height==yuv.uv_height) { // NYI
	} else if (yuv.y_width==yuv.uv_width*2 && yuv.y_height==yuv.uv_height) { // NYI
	} else if (yuv.y_width==yuv.uv_width && yuv.y_height==yuv.uv_height*2) { // NYI
	} else if (yuv.y_width==yuv.uv_width*2 && yuv.y_height==yuv.uv_height*2) {
		int i2,j2;
		int w2,h2;
		unsigned char* py;
		unsigned char *pu,*pv;
		unsigned int* po;
		unsigned int c;
		int pit;
		po=(unsigned int*)tbm->data;
		py=yuv.y;
		pu=yuv.u;
		pv=yuv.v;
		pit=tbm->size.x;//lr.Pitch>>2;
		w2=yuv.y_width/2;
		h2=yuv.y_height/2;
		for (j2=0,j=0;j2<h2;++j2,j+=2) {
			for (i2=0,i=0;i2<w2;++i2,i+=2) {
				colorconv(py[i],pu[i2],pv[i2],&c);
				po[i]=c;
				colorconv(py[i+1],pu[i2],pv[i2],&c);
				po[i+1]=c;
			}
			py+=yuv.y_stride;
			po+=pit;
			for (i2=0,i=0;i2<w2;++i2,i+=2) {
				colorconv(py[i],pu[i2],pv[i2],&c);
				po[i]=c;
				colorconv(py[i+1],pu[i2],pv[i2],&c);
				po[i+1]=c;
			}
			py+=yuv.y_stride;
			pu+=yuv.uv_stride;
			pv+=yuv.uv_stride;
			po+=pit;
		}
		++moviefrm;
	}

//	hr=IDirect3DTexture8_UnlockRect(movietx->t,0);
//	if (hr!=D3D_OK)
//		errorexit("can't unlock movie tex %x",hr);
//	if (!whplaying) {
//		playasound(wh);
//		whplaying=1;
//	}
	if (doplay) {
	   wave_play(wh,1);
	   doplay=0;
	}
	outtextxyf32(tbm,8,8,C32WHITE,"%d",cnt++);
	return tbm;
}

/*void moviedrawframe(int x,int y,int w,int h)
{
	struct rgb32 c={0,0,0,0};
	d3d_setspriteuvs(0,0,400.0f/512.0f,300.0f/512.0f);
	d3d_drawsprite(0,0,c,x,y,w,h,movietx->t);
	d3d_resetspriteuvs();
}
*/
void movieexit()
{
	if(theora_p){
		ogg_stream_clear(&to);
		theora_clear(&td);
		theora_comment_clear(&tc);
		theora_info_clear(&ti);
		theora_p=0;
	}
	ogg_sync_clear(&oy);

	if(infile && infile!=stdin) {
		fclose(infile);
		infile=0;
	}
	wave_unload(wh);
	wh=0;
	doplay=0;
	if (tbm) {
		bitmap32free(tbm);
		cnt=0;
		tbm=0;
	}
}
#endif
