/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <string>

namespace rikitiki {
  namespace ContentType {
    enum t {
      DEFAULT = -2,
      ALL = -1, 
      application_atom_xml,
      application_ecmascript,
      application_EDI_X12,
      application_EDIFACT,
      application_json,
      application_javascript,
      application_octet_stream,
      application_ogg,
      application_pdf,
      application_postscript,
      application_rdf_xml,
      application_rss_xml,
      application_soap_xml,
      application_font_woff,
      application_xhtml_xml,
      application_xml,
      application_xml_dtd,
      application_xop_xml,
      application_zip,
      application_gzip,

      audio_basic,
      audio_L24,
      audio_mp4,
      audio_mpeg,
      audio_ogg,
      audio_vorbis,
      audio_vnd_rn_realaudio,
      audio_vnd_wave,
      audio_webm,

      image_gif,
      image_jpeg,
      image_pjpeg,
      image_png,
      image_svg_xml,
      image_tiff,
      image_vnd_microsoft_icon,

      message_http,
      message_imdn_xml,
      message_partial,
      message_rfc822,

      model_example,
      model_iges,
      model_mesh,
      model_vrml,
      model_x3d_binary,
      model_x3d_vrml,
      model_x3d_xml,

      multipart_mixed,
      multipart_alternative,
      multipart_related,
      multipart_form_data,
      multipart_signed,
      multipart_encrypted,

      text_cmd,
      text_css,
      text_csv,
      text_html,
      text_javascript,
      text_plain,
      text_vcard,
      text_xml,

      video_mpeg,
      video_mp4,
      video_ogg,
      video_quicktime,
      video_webm,
      video_x_matroska,
      video_x_ms_wmv,
      video_x_flv,
      MAX
    };

    t FromString(const std::wstring& type);
    t FromString(const std::string& type);

    std::wstring ToString(t type);
    std::wistream& operator >>(std::wistream& s, t& _t);


  }
}

