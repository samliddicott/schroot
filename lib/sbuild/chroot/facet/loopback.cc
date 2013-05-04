/* Copyright © 2005-2013  Roger Leigh <rleigh@debian.org>
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

#include <sbuild/chroot/facet/factory.h>
#include <sbuild/chroot/facet/fsunion.h>
#include <sbuild/chroot/facet/loopback.h>
#include <sbuild/chroot/facet/mountable.h>
#include <sbuild/chroot/facet/session.h>
#include <sbuild/format-detail.h>

#include <cassert>
#include <cerrno>

#include <boost/format.hpp>

using std::endl;
using boost::format;
using namespace sbuild;

namespace sbuild
{
  namespace chroot
  {
    namespace facet
    {

      namespace
      {

        factory::facet_info loopback_info =
          {
            "loopback",
            N_("Support for ‘loopback’ chroots"),
            []() -> facet::ptr { return loopback::create(); }
          };

        factory loopback_register(loopback_info);

      }

      loopback::loopback ():
        storage(),
        filename()
      {
      }

      loopback::loopback (const loopback& rhs):
        storage(rhs),
        filename(rhs.filename)
      {
      }

      loopback::~loopback ()
      {
      }

      void
      loopback::set_chroot (chroot& chroot,
                            bool    copy)
      {
        storage::set_chroot(chroot);
        if (!copy && !owner->get_facet<mountable>())
          owner->add_facet(mountable::create());
#ifdef SBUILD_FEATURE_UNION
        if (!copy && !owner->get_facet<fsunion>())
          owner->add_facet(fsunion::create());
#endif // SBUILD_FEATURE_UNION
      }

      std::string const&
      loopback::get_name () const
      {
        static const std::string name("loopback");

        return name;
      }

      loopback::ptr
      loopback::create ()
      {
        return ptr(new loopback());
      }

      facet::ptr
      loopback::clone () const
      {
        return ptr(new loopback(*this));
      }

      std::string const&
      loopback::get_filename () const
      {
        return this->filename;
      }

      void
      loopback::set_filename (std::string const& filename)
      {
        if (!is_absname(filename))
          throw error(filename, chroot::FILE_ABS);

        this->filename = filename;
      }

      std::string
      loopback::get_path () const
      {
        mountable::const_ptr pmnt
          (owner->get_facet<mountable>());

        std::string path(owner->get_mount_location());

        if (pmnt)
          path += pmnt->get_location();

        return path;
      }

      void
      loopback::setup_env (environment& env) const
      {
        storage::setup_env(env);

        env.add("CHROOT_FILE", get_filename());
      }

      void
      loopback::setup_lock (chroot::setup_type type,
                            bool               lock,
                            int                status)
      {
        // Check ownership and permissions.
        if (type == chroot::SETUP_START && lock == true)
          {
            stat file_status(this->filename);

            // NOTE: taken from chroot_config::check_security.
            if (file_status.uid() != 0)
              throw error(this->filename, chroot::FILE_OWNER);
            if (file_status.check_mode(stat::PERM_OTHER_WRITE))
              throw error(this->filename, chroot::FILE_PERMS);
            if (!file_status.is_regular())
              throw error(this->filename, chroot::FILE_NOTREG);
          }

        /* Create or unlink session information. */
        if ((type == chroot::SETUP_START && lock == true) ||
            (type == chroot::SETUP_STOP && lock == false && status == 0))
          {
            bool start = (type == chroot::SETUP_START);
            owner->get_facet_strict<session>()->setup_session_info(start);
          }
      }

      void
      loopback::get_details (format_detail& detail) const
      {
        storage::get_details(detail);

        if (!this->filename.empty())
          detail.add(_("File"), get_filename());
      }

      void
      loopback::get_used_keys (string_list& used_keys) const
      {
        storage::get_used_keys(used_keys);

        used_keys.push_back("file");
      }

      void
      loopback::get_keyfile (keyfile& keyfile) const
      {
        storage::get_keyfile(keyfile);

        keyfile::set_object_value(*this, &loopback::get_filename,
                                  keyfile, owner->get_name(), "file");
      }

      void
      loopback::set_keyfile (keyfile const& keyfile)
      {
        storage::set_keyfile(keyfile);

        keyfile::get_object_value(*this, &loopback::set_filename,
                                  keyfile, owner->get_name(), "file",
                                  keyfile::PRIORITY_REQUIRED);
      }

      void
      loopback::chroot_session_setup (chroot const&      parent,
                                      std::string const& session_id,
                                      std::string const& alias,
                                      std::string const& user,
                                      bool               root)
      {
        // Loopback chroots need the mount device name specifying.
        mountable::ptr pmnt
          (owner->get_facet<mountable>());
        if (pmnt)
          pmnt->set_mount_device(get_filename());
      }

    }
  }
}
