/*
MIT License

Copyright (c) 2025 Patryk Czachurski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <array>
#include <vector>
#include "rect_structs.h"

namespace rectpack2D {
	class default_empty_spaces {
		std::vector<space_rect> empty_spaces;

	public:
		void remove(const int i) {
			empty_spaces[i] = empty_spaces.back();
			empty_spaces.pop_back();
		}

		bool add(const space_rect r) {
			empty_spaces.emplace_back(r);
			return true;
		}

		auto get_count() const {
			return empty_spaces.size();
		}

		void reset() {
			empty_spaces.clear();
		}

		const auto& get(const int i) {
			return empty_spaces[i];
		}
	};

	template <int MAX_SPACES>
	class static_empty_spaces {
		int count_spaces = 0;
		std::array<space_rect, MAX_SPACES> empty_spaces;

	public:
		void remove(const int i) {
			empty_spaces[i] = empty_spaces[count_spaces - 1];
			--count_spaces;
		}

		bool add(const space_rect r) {
			if (count_spaces < static_cast<int>(empty_spaces.size())) {
				empty_spaces[count_spaces] = r;
				++count_spaces;

				return true;
			}

			return false;
		}

		auto get_count() const {
			return count_spaces;
		}

		void reset() {
			count_spaces = 0;
		}

		const auto& get(const int i) {
			return empty_spaces[i];
		}
	};
}