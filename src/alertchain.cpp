#include <eosio/eosio.hpp>

using namespace eosio;
using namespace std;


class [[eosio::contract("alertchain")]] alertchain : public eosio::contract
{
	public:
		alertchain(name receiver, name code, datastream<const char*> ds):
	contract(receiver, code, ds) {}
		
		[[eosio::action]]
		void upsert(name user, std::string first_name, std::string last_name, int zipcode) {
			require_auth( user );
			address_index addresses(get_self(), get_first_receiver().value);
			auto iterator = addresses.find(user.value);
			// if user does not exist, add to table
            if( iterator == addresses.end() )
			{
				addresses.emplace(user, [&]( auto& row ) {
					row.key = user;
					row.first_name = first_name;
					row.last_name = last_name;
					row.zipcode = zipcode;
                    row.nominations = 0;
				});
			}
            // else user does exist and can modify their information 
            // user cannot modify their own nomination count
			else {
				addresses.modify(iterator, user, [&]( auto& row  ){
					row.key = user;
					row.first_name = first_name;
					row.last_name = last_name;
					row.zipcode = zipcode;
				});
			}
		}
		
		[[eosio::action]]
		void erase(name user){
			require_auth(user);
			address_index addresses(get_self(), get_first_receiver().value);
			auto iterator = addresses.find(user.value);
			check(iterator != addresses.end(), "User does note exist");
			addresses.erase(iterator);
		}

		[[eosio::action]]
		string alert(name user, string testdata){
			require_auth(user);
			printf("%s\n",testdata.c_str());
			return testdata;
		}

	private:
		struct [[eosio::table]] person {
			name key;
			std::string first_name;
			std::string last_name;
			int zipcode;
            int nominations;

			uint64_t primary_key() const { return key.value;}
		};
		
		typedef eosio::multi_index<"people"_n, person> address_index;
};


