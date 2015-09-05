#include "cgimap/api06/changeset_handler.hpp"
#include "cgimap/request_helpers.hpp"
#include "cgimap/http.hpp"
#include "cgimap/config.hpp"

#include <sstream>

using std::stringstream;
using std::vector;

namespace api06 {

changeset_responder::changeset_responder(mime::type mt, osm_id_t id_,
                                         bool include_discussion_,
                                         factory_ptr &w_)
  : osm_current_responder(mt, w_), id(id_),
    include_discussion(include_discussion_) {
  vector<osm_id_t> ids;
  ids.push_back(id);

#ifdef ENABLE_EXPERIMENTAL
  if (!sel->supports_changesets()) {
#endif /* ENABLE_EXPERIMENTAL */
    throw http::server_error("Data source does not support changesets.");
#ifdef ENABLE_EXPERIMENTAL
  }

  if (sel->select_changesets(ids) == 0) {
    throw http::not_found("");
  }

  if (include_discussion) {
    sel->select_changeset_discussions();
  }
#endif /* ENABLE_EXPERIMENTAL */
}

changeset_responder::~changeset_responder() {}

changeset_handler::changeset_handler(request &req, osm_id_t id_)
  : id(id_), include_discussion(false) {
  using std::map;
  using std::string;

  string decoded = http::urldecode(get_query_string(req));
  const map<string, string> params = http::parse_params(decoded);
  map<string, string>::const_iterator itr = params.find("include_discussion");

  include_discussion = (itr != params.end());
}

changeset_handler::~changeset_handler() {}

std::string changeset_handler::log_name() const { return "changeset"; }

responder_ptr_t changeset_handler::responder(factory_ptr &w) const {
  return responder_ptr_t(new changeset_responder(mime_type, id, include_discussion, w));
}

} // namespace api06
