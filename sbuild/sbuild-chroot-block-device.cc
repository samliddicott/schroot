/* Copyright © 2005-2008  Roger Leigh <rleigh@debian.org>
 *
 * schroot is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * schroot is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 *********************************************************************/

#include <config.h>

#include "sbuild-chroot-block-device.h"
#include "sbuild-format-detail.h"
#include "sbuild-lock.h"
#include "sbuild-util.h"

#include <cerrno>
#include <cstring>

#include <boost/format.hpp>

using boost::format;
using namespace sbuild;

chroot_block_device::chroot_block_device ():
#ifdef SBUILD_FEATURE_UNION
  chroot_session(),
#endif // SBUILD_FEATURE_UNION
  chroot_block_device_base()
#ifdef SBUILD_FEATURE_UNION
  , chroot_union()
#endif // SBUILD_FEATURE_UNION
{
}

chroot_block_device::~chroot_block_device ()
{
}

chroot_block_device::chroot_block_device (const chroot_block_device& rhs):
#ifdef SBUILD_FEATURE_UNION
  chroot_session(rhs),
#endif // SBUILD_FEATURE_UNION
  chroot_block_device_base(rhs)
#ifdef SBUILD_FEATURE_UNION
  , chroot_union(rhs)
#endif // SBUILD_FEATURE_UNION
{
}

chroot_block_device::chroot_block_device (const chroot_lvm_snapshot& rhs):
#ifdef SBUILD_FEATURE_UNION
  chroot_session(rhs),
#endif // SBUILD_FEATURE_UNION
  chroot_block_device_base(rhs)
{
}

sbuild::chroot::ptr
chroot_block_device::clone () const
{
  return ptr(new chroot_block_device(*this));
}

#ifdef SBUILD_FEATURE_UNION
sbuild::chroot::ptr
chroot_block_device::clone_session (std::string const& session_id) const
{
  ptr session;

  if (get_union_configured())
    {
      session = ptr(new chroot_block_device(*this));
      clone_session_setup(session, session_id);
    }

  return session;
}

sbuild::chroot::ptr
chroot_block_device::clone_source () const
{
  ptr clone;

  if (get_union_configured())
    {
      clone = ptr(new chroot_block_device(*this));
      clone_source_setup(clone);
    }

  return clone;
}
#endif // SBUILD_FEATURE_UNION

void
chroot_block_device::setup_env (environment& env)
{
  chroot_block_device_base::setup_env(env);
#ifdef SBUILD_FEATURE_UNION
  chroot_union::setup_env(env);
#endif // SBUILD_FEATURE_UNION
}

sbuild::chroot::session_flags
chroot_block_device::get_session_flags () const
{
  return chroot_block_device_base::get_session_flags()
#ifdef SBUILD_FEATURE_UNION
    | chroot_union::get_session_flags()
#endif // SBUILD_FEATURE_UNION
    ;
}

void
chroot_block_device::get_details (format_detail& detail) const
{
  chroot_block_device_base::get_details(detail);
#ifdef SBUILD_FEATURE_UNION
  chroot_union::get_details(detail);
#endif // SBUILD_FEATURE_UNION
}

void
chroot_block_device::get_keyfile (keyfile& keyfile) const
{
  chroot_block_device_base::get_keyfile(keyfile);
  chroot_mountable::get_keyfile(keyfile);
#ifdef SBUILD_FEATURE_UNION
  chroot_union::get_keyfile(keyfile);
#endif // SBUILD_FEATURE_UNION
}

void
chroot_block_device::set_keyfile (keyfile const& keyfile,
				  string_list&   used_keys)
{
  chroot_block_device_base::set_keyfile(keyfile, used_keys);
#ifdef SBUILD_FEATURE_UNION
  chroot_union::set_keyfile(keyfile, used_keys);
#endif // SBUILD_FEATURE_UNION
}
