
#pragma once
#include <slankdev/util.h>
#include <slankdev/string.h>
#include "gobgp.grpc.pb.h"


inline std::string inet_nlri2str(const std::string& nlri)
{
  assert(nlri.size() > 1);
  uint8_t prefix = nlri[0];
  uint32_t addr = 0;
  union {
    uint8_t u8[4];
    uint32_t u32;
  } U; U.u32 = 0;
  constexpr size_t inet_oct_max = 4;
  for (size_t i=0; i<inet_oct_max; i++) {
    U.u8[i] = nlri[i+1];
    if (i+2 >= nlri.size()) break;
  }
  return slankdev::format("%d.%d.%d.%d/%u",
      U.u8[0], U.u8[1], U.u8[2], U.u8[3], prefix);
}

std::string Path2str(const gobgpapi::Path& path)
{
  std::string nexthop = path.neighbor_ip();
  if (nexthop == "<nil>") nexthop = "0.0.0.0";
  std::string str = "";
  str += slankdev::format("[%s] ", path.is_withdraw()?"DELROUTE":"ROUTE");
  str += slankdev::format("%s via %s aspath [%u]",
      inet_nlri2str(path.nlri()).c_str(),
      nexthop.c_str(), path.source_asn());
  return str;
}

inline void print_RPKIValidation(FILE* fp, const gobgpapi::RPKIValidation& val)
{
  using namespace slankdev;
  depth_fprintf_pusher p;
  depth_fprintf(fp, "<no-impl>\n");
}

inline void print_Path(FILE* fp, const gobgpapi::Path& path)
{
  using namespace slankdev;
  depth_fprintf_pusher p;

  depth_fprintf(fp, "nlri: %s \n", inet_nlri2str(path.nlri()).c_str());
  depth_fprintf(fp, "pattrs: [\n");
  for (size_t i=0, n=path.pattrs().size(); i<n; i++) {
    depth_fprintf_pusher p;
    depth_fprintf(fp, "[%zd]: ", i);
    for (size_t j=0, m=path.pattrs()[i].size(); j<m; j++)
      printf("%02x ", path.pattrs()[i][j]);
    fprintf(fp, "\n");

  }
  depth_fprintf(fp, "]\n");
  depth_fprintf(fp, "age: %u \n", path.age());
  depth_fprintf(fp, "best: %s \n", path.best()?"true":"false");
  depth_fprintf(fp, "is_withdraw: %s\n", path.is_withdraw()?"true":"false");
  depth_fprintf(fp, "validation: %u\n", path.validation());

  depth_fprintf(fp, "validation_detail: {\n");
  print_RPKIValidation(fp, path.validation_detail());
  depth_fprintf(fp, "}\n");

  depth_fprintf(fp, "no_implicit_withdraw: %s\n", path.no_implicit_withdraw()?"true":"false");
  depth_fprintf(fp, "family: %u \n", path.family());
  depth_fprintf(fp, "source_asn: %u \n", path.source_asn());
  depth_fprintf(fp, "source_id: %s\n", path.source_id().c_str());
  depth_fprintf(fp, "filtered: %s \n", path.filtered()?"true":"false");
  depth_fprintf(fp, "stale: %s \n", path.stale()?"true":"false");
  depth_fprintf(fp, "is_from_external: %s \n", path.is_from_external()?"true":"false");
  depth_fprintf(fp, "neighbor_ip: %s\n", path.neighbor_ip().c_str());
  depth_fprintf(fp, "uuid: " );
  for (size_t i=0,n=path.uuid().size(); i<n; i++)
    fprintf(fp, "%02x%s", path.uuid()[i], (i+1)<n?" ":"\n");

  depth_fprintf(fp, "is_nexthop_invalid: %s\n", path.is_nexthop_invalid()?"true":"false");
  depth_fprintf(fp, "identifier: %u\n", path.identifier());
  depth_fprintf(fp, "local_identifier: %u\n", path.local_identifier());
}


