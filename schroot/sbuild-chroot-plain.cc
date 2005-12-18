/* sbuild-chroot-plain - sbuild simple chroot object
 *
 * Copyright © 2005  Roger Leigh <rleigh@debian.org>
 *
 * schroot is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * schroot is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA  02111-1307  USA
 *
 *********************************************************************/

/**
 * SECTION:sbuild-chroot-plain
 * @short_description: simple chroot object
 * @title: ChrootPlain
 *
 * This object represents a chroot located on a mounted filesystem.
 */

#include <config.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gi18n.h>

#include "sbuild-chroot-plain.h"
#include "sbuild-keyfile.h"

using namespace sbuild;

ChrootPlain::ChrootPlain():
  Chroot(),
  location()
{
}

ChrootPlain::ChrootPlain (GKeyFile           *keyfile,
			  const std::string&  group):
  Chroot(keyfile, group),
  location()
{
}

ChrootPlain::~ChrootPlain()
{
}

Chroot *
ChrootPlain::clone () const
{
  return new ChrootPlain(*this);
}

/**
 * sbuild_chroot_plain_get_location:
 * @chroot: an #ChrootPlain
 *
 * Get the directory location of the chroot.
 *
 * Returns a string. This string points to internally allocated
 * storage in the chroot and must not be freed, modified or stored.
 */
const std::string&
ChrootPlain::get_location () const
{
  return this->location;
}

/**
 * sbuild_chroot_plain_set_location:
 * @chroot: an #ChrootPlain.
 * @location: the location to set.
 *
 * Set the directory location of a chroot.
 */
void
ChrootPlain::set_location (const std::string& location)
{
  this->location = location;
}

const std::string&
ChrootPlain::get_mount_location () const
{
  return this->location;
}

const std::string&
ChrootPlain::get_chroot_type () const
{
  static const std::string type("plain");

  return type;
}

void
ChrootPlain::setup_env (env_list& env)
{
  this->Chroot::setup_env(env);

  setup_env_var(env, "CHROOT_LOCATION",
		get_location());
}

void
ChrootPlain::setup_lock (Chroot::SetupType type,
			 bool              lock)
{
  /* By default, plain chroots do no locking. */
}

Chroot::SessionFlags
ChrootPlain::get_session_flags () const
{
  return static_cast<SessionFlags>(0);
}

void
ChrootPlain::print_details (FILE *file) const
{
  this->Chroot::print_details(file);

  if (!this->location.empty())
    g_fprintf(file, "  %-22s%s\n", _("Location"), this->location.c_str());
}

void
ChrootPlain::print_config (FILE *file) const
{
  this->Chroot::print_config(file);

  g_fprintf(file, _("location=%s\n"), this->location.c_str());
}

void
ChrootPlain::read_keyfile (GKeyFile           *keyfile,
			   const std::string&  group)
{
  std::string location;
  if (keyfile_read_string(keyfile, group, "location", location))
    set_location(location);
}


/*
 * Local Variables:
 * mode:C++
 * End:
 */
