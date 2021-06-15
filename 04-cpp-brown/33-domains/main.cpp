#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

bool IsSubdomain(const string& a, const string& b) {
  auto [a_it, b_it] = mismatch(a.rbegin(), a.rend(), b.rbegin(), b.rend());
  bool a_end = (a_it == a.rend()), b_end = (b_it == b.rend());
  return (a_end && b_end) || (a_end && *b_it == '.') || (b_end && *a_it == '.');
}

vector<string> ReadDomains(istream& input) {
  size_t count;
  input >> count;
  vector<string> domains(count);
  for (string& domain : domains) {
    getline(input >> ws, domain);
  }
  return domains;
}

int main() {
  auto comparator = [](const string& a, const string& b) {
    return lexicographical_compare(a.rbegin(), a.rend(), b.rbegin(), b.rend());
  };
  vector<string> banned_domains = ReadDomains(cin);
  sort(banned_domains.begin(), banned_domains.end(), comparator);

  auto it = unique(banned_domains.begin(), banned_domains.end(), IsSubdomain);
  banned_domains.erase(it, banned_domains.end());

  for (const string& domain : ReadDomains(cin)) {
    if (const auto it = upper_bound(banned_domains.begin(),
                                    banned_domains.end(), domain, comparator);
        it != banned_domains.begin() && IsSubdomain(domain, *prev(it))) {
      cout << "Bad\n";
    } else {
      cout << "Good\n";
    }
  }

  return 0;
}
