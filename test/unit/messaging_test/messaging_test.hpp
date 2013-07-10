/* libalgol - a collection of plug-ins for developing back-end C++ web tools
 * Copyright (c) 2013 Algol Labs, LLC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H_ALGOL_messaging_test_H
#define H_ALGOL_messaging_test_H

#include "test.hpp"
#include "algol/algol.hpp"
 #include "algol/messaging/communicator.hpp"

namespace algol {

	class messaging_test : public test, public communicator {
	public:
		messaging_test();
		virtual ~messaging_test();

    int run(int argc, char** argv);

	protected:
    virtual void on_message_received(const message&);
    virtual void on_message_sent(const message&, comm_rc);
	};

}
#endif
