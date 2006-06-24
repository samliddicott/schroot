/* Copyright © 2005-2006  Roger Leigh <rleigh@debian.org>
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

#ifndef SBUILD_FORMAT_DETAIL_H
#define SBUILD_FORMAT_DETAIL_H

#include <sbuild/sbuild-config.h>
#include <sbuild/sbuild-types.h>
#include <sbuild/sbuild-util.h>

#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

namespace sbuild
{

  /**
   * Format names and values for output.
   */
  template<typename T>
  class format_detail;

  /**
   * Output the formatted detail to an ostream.
   *
   * @param stream the stream to output to.
   * @param rhs the formatted detail to output.
   * @returns the stream.
   */
  template<typename T> std::ostream&
  operator << (std::ostream&           stream,
	       format_detail<T> const& rhs);

  /**
   * Output the formatted detail to an ostream.  This is a special
   * case for boolean values.
   *
   * @param stream the stream to output to.
   * @param rhs the formatted detail to output.
   * @returns the stream.
   */
  template<> std::ostream&
  operator << (std::ostream&              stream,
	       format_detail<bool> const& rhs);

  /**
   * Output the formatted detail to an ostream.  This is a special
   * case for string_list values.
   *
   * @param stream the stream to output to.
   * @param rhs the formatted detail to output.
   * @returns the stream.
   */
  template<> std::ostream&
  operator << (std::ostream&                     stream,
	       format_detail<string_list> const& rhs);

  /**
   * Helper to perform formatting of chroot details.
   */
  template<typename T>
  class format_detail
  {
    /**
     * The constructor.
     *
     * @param name the name of the property to format.
     * @param value the value of the property to format.  The value
     * type must support output to an ostream.
     */
  public:
    format_detail (std::string const& name,
		   T const&           value):
      name(name),
      value(value)
    {}

      friend std::ostream&
      operator << <>(std::ostream&, format_detail<T> const&);

  private:
    /// The name of the property.
    std::string const& name;
    /// The value of the property.
    T const&           value;
  };

  template<typename T>
  inline std::ostream&
  operator << (std::ostream&           stream,
	       format_detail<T> const& rhs)
  {
    std::locale loc = stream.getloc();
    std::wostringstream ws;
    ws.imbue(loc);
    ws << L"  " <<
      std::setw(21) << std::left << widen_string(rhs.name, loc);

    return stream << narrow_string(ws.str(), loc)
		  << ' ' << rhs.value << '\n';

  }

  template<>
  inline std::ostream&
  operator << (std::ostream&                     stream,
	       format_detail<string_list> const& rhs)
  {
    return stream <<
      format_detail<std::string>(rhs.name,
				 string_list_to_string(rhs.value, " "));
  }

  /**
   * Format a name-value pair for output.  This is a convenience
   * wrapper to construct a format_detail of the appropriate type.
   *
   * @param name the name to output.
   * @param value the value to output.
   * @returns a format_detail of the appropriate type.
   */
  template<typename T>
  inline format_detail<T>
  format_details (std::string const& name,
		  T const&           value)
  {
    return format_detail<T>(name, value);
  }

}

#endif /* SBUILD_FORMAT_DETAIL_H */

/*
 * Local Variables:
 * mode:C++
 * End:
 */
