/*
  rexgen - a tool to create words based on regular expressions
  Copyright (C) 2012-2014  Jan Starke <jan.starke@outofbed.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110, USA
*/

#include <algorithm>
#include <librexgen/iterator/iteratorstate.h>
#include <librexgen/regex/regex.h>
#include <librexgen/regex/regexalternatives.h>
#include <librexgen/iterator/regexalternativesiterator.h>
#include <librexgen/iterator/caseiterator.h>
using namespace std;

Iterator* RegexAlternatives::singleIterator(IteratorState* state) const {
  if (children() == 1) {
    return firstChild()->iterator(state);
  }

  RegexAlternativesIterator* rai = new RegexAlternativesIterator(getId());
  mapToChildren([&rai, state](const Regex* re){
    rai->addChild(re->iterator(state));});

  return rai;
}

Iterator* RegexAlternatives::iterator(IteratorState* state) const {
  Iterator* iter = NULL;

	if (! state->getUseRegexBackreferences()) {
		iter = state->getIterator(getGroupId());
		if (iter != NULL) {
			return iter;
		}
	}

  if (children() == 1) {
    if (getMinOccurs() == 1 && getMaxOccurs() == 1) {
      iter = firstChild()->iterator(state);
    } else {
      iter = new IteratorPermuter(
              firstChild()->getId(), firstChild(), state, getMinOccurs(), getMaxOccurs());
    }
  } else {
    iter = RegexContainer::iterator(state);
  }

  if (getGroupId() > 0) {
    state->registerIterator(getGroupId(), iter);
  }

  if (handle_case != CASE_IGNORE) {
    iter = new CaseIterator(iter, handle_case);
  }

  return iter;
}
