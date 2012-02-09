/*
 * Copyright (C) 2010-2011 Stefan Sayer
 *
 * This file is part of SEMS, a free SIP media server.
 *
 * SEMS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * For a license to use the SEMS software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * SEMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _SBCCallProfile_h
#define _SBCCallProfile_h

#include "AmConfigReader.h"
#include "HeaderFilter.h"
#include "ampi/UACAuthAPI.h"
#include "ParamReplacer.h"

#include <set>
#include <string>
#include <map>
#include <list>

using std::string;
using std::map;
using std::set;

typedef map<string, AmArg>   SBCVarMapT;
typedef SBCVarMapT::iterator SBCVarMapIteratorT;

struct CCInterface {
  string cc_name;
  string cc_module;
  map<string, string> cc_values;

  CCInterface(string cc_name)
  : cc_name(cc_name) { }
  CCInterface() { }
};

typedef std::list<CCInterface> CCInterfaceListT;
typedef CCInterfaceListT::iterator CCInterfaceListIteratorT;

class PayloadDesc {
  protected:
    std::string name;
    unsigned clock_rate; // 0 means "doesn't matter"

  public:
    bool match(const SdpPayload &p) const;
    std::string print() const;
    bool operator==(const PayloadDesc &other) const;

    /* FIXME: really want all of this?
     * reads from format: name/clock_rate, nothing need to be set
     * for example: 
     *	  PCMU
     *	  bla/48000
     *	  /48000
     * */
    bool read(const std::string &s);
};

struct SBCCallProfile
  : public AmObject {
  string md5hash;
  string profile_file;

  string ruri;       /* updated if set */
  string from;       /* updated if set */
  string to;         /* updated if set */
  string contact;

  string callid;

  string outbound_proxy;
  bool force_outbound_proxy;

  string next_hop_ip;
  string next_hop_port;
  unsigned short next_hop_port_i;
  string next_hop_for_replies;

  FilterType headerfilter;
  set<string> headerfilter_list;

  FilterType messagefilter;
  set<string> messagefilter_list;

  bool sdpfilter_enabled;
  FilterType sdpfilter;
  set<string> sdpfilter_list;
  bool anonymize_sdp;

  bool sdpalinesfilter_enabled;
  FilterType sdpalinesfilter;
  set<string> sdpalinesfilter_list;

  string sst_enabled;
  bool sst_enabled_value;
  string sst_aleg_enabled;
  AmConfigReader sst_a_cfg;    // SST config (A leg)
  AmConfigReader sst_b_cfg;    // SST config (B leg)

  bool auth_enabled;
  UACAuthCred auth_credentials;

  bool auth_aleg_enabled;
  UACAuthCred auth_aleg_credentials;

  CCInterfaceListT cc_interfaces;

  SBCVarMapT cc_vars;

  map<unsigned int, std::pair<unsigned int, string> > reply_translations;

  string append_headers;

  string refuse_with;

  bool rtprelay_enabled;
  string force_symmetric_rtp;
  bool force_symmetric_rtp_value;
  bool msgflags_symmetric_rtp;
  bool rtprelay_transparent_seqno;
  bool rtprelay_transparent_ssrc;

  string rtprelay_interface;
  int rtprelay_interface_value;
  string aleg_rtprelay_interface;
  int aleg_rtprelay_interface_value;

  string outbound_interface;
  int outbound_interface_value;

  std::vector<PayloadDesc> payload_order;
  bool readPayloadOrder(const std::string &src);
  void orderSDP(AmSdp& sdp); // do the SDP changes

  // todo: RTP transcoding mode

  SBCCallProfile()
  : headerfilter(Transparent),
    messagefilter(Transparent),
    sdpfilter_enabled(false),
    sdpfilter(Transparent),
    sdpalinesfilter_enabled(false),
    sdpalinesfilter(Transparent),
    auth_enabled(false),
    next_hop_port_i(0),
    sst_enabled_value(false),
    rtprelay_enabled(false),
    force_symmetric_rtp_value(false),
    rtprelay_transparent_seqno(true),
    rtprelay_transparent_ssrc(true),
    rtprelay_interface_value(-1),
    aleg_rtprelay_interface_value(-1),
    outbound_interface_value(-1)
  { }

  ~SBCCallProfile()
  { }

  bool readFromConfiguration(const string& name, const string profile_file_name);

  bool operator==(const SBCCallProfile& rhs) const;
  string print() const;

  bool evaluate(const AmSipRequest& req,
      const string& app_param,
      AmUriParser& ruri_parser, AmUriParser& from_parser,
      AmUriParser& to_parser);

};

#endif // _SBCCallProfile_h
