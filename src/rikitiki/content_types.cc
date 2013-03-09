/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/content_types.h>

#define CASE_RET(X,Y) case X ## _ ## Y: return #X "/" #Y;
#define CASE_RET_PLUS(X,Y,Z) case X ## _ ## Y ## _ ## Z: return #X "/" #Y "+" #Z;
#define CASE_RET_DASH(X,Y,Z) case X ## _ ## Y ## _ ## Z: return #X "/" #Y "-" #Z;
#define CASE_RET_DASH_DASH(X,Y,Z,A) case X ## _ ## Y ## _ ## Z ## _ ## A: return #X "/" #Y "-" #Z "-" #A;


namespace rikitiki {
  namespace ContentType {
    std::string ToString(t type){
      switch(type){
	CASE_RET_PLUS(application, atom, xml);
	CASE_RET     (application, ecmascript);
	CASE_RET_DASH(application, EDI, X12);
	CASE_RET     (application, EDIFACT);
	CASE_RET     (application, json);
	CASE_RET     (application, javascript);
	CASE_RET_DASH(application, octet, stream);
	CASE_RET     (application, ogg);
	CASE_RET     (application, pdf);
	CASE_RET     (application, postscript);
	CASE_RET_PLUS(application, rdf, xml);
	CASE_RET_PLUS(application, rss, xml);
	CASE_RET_PLUS(application, soap, xml);
	CASE_RET_DASH(application, font, woff);
	CASE_RET_PLUS(application, xhtml, xml);
	CASE_RET     (application, xml);
	CASE_RET_DASH(application, xml, dtd);
	CASE_RET_PLUS(application, xop, xml);
	CASE_RET     (application, zip);
	CASE_RET     (application, gzip);
	CASE_RET     (audio, basic);
	CASE_RET     (audio, L24);
	CASE_RET     (audio, mp4);
	CASE_RET     (audio, mpeg);
	CASE_RET     (audio, ogg);
	CASE_RET     (audio, vorbis);
	CASE_RET     (audio, webm);

	CASE_RET     (image, gif);
	CASE_RET     (image, jpeg);
	CASE_RET     (image, pjpeg);
	CASE_RET     (image, png);
	CASE_RET_PLUS(image, svg, xml);
	CASE_RET     (image, tiff);

	CASE_RET     (message, http);
	CASE_RET_PLUS(message, imdn, xml);
	CASE_RET     (message, partial);
	CASE_RET     (message, rfc822);

	CASE_RET     (model, example);
	CASE_RET     (model, iges);
	CASE_RET     (model, mesh);
	CASE_RET     (model, vrml);
	CASE_RET_PLUS(model, x3d, binary);
	CASE_RET_PLUS(model, x3d, vrml);
	CASE_RET_PLUS(model, x3d, xml);

	CASE_RET     (multipart, mixed);
	CASE_RET     (multipart, alternative);
	CASE_RET     (multipart, related);
	CASE_RET_DASH(multipart, form, data);
	CASE_RET     (multipart, signed);
	CASE_RET     (multipart, encrypted);

	CASE_RET     (text, cmd);
	CASE_RET     (text, css);
	CASE_RET     (text, csv);

	CASE_RET     (text, javascript);
	CASE_RET     (text, plain);
	CASE_RET     (text, vcard);
	CASE_RET     (text, xml);

	CASE_RET     (video, mpeg);
	CASE_RET     (video, mp4);
	CASE_RET     (video, ogg);
	CASE_RET     (video, quicktime);
	CASE_RET     (video, webm);
	CASE_RET_DASH(video, x, matroska);
	CASE_RET_DASH_DASH(video, x, ms, wmv);
	CASE_RET_DASH(video, x, flv);
      case DEFAULT:
	CASE_RET     (text, html);
      default:
	return "unknown";
	  }
    }
  }
}
