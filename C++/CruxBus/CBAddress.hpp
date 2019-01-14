/*

        TODO: The address class will eventually hold information regarding the address'
              key information, along with other operating information.
             
*/

#ifndef CBADDRESS_HEADER_G
#define CBADDRESS_HEADER_G

#include <inttypes.h>
#include <list>

//!
//! \def A default address for a CB::Address object
//!
#define CB_DEFAULT_ADDRESS 0xFFFFFFFF

//!
//! \file CBAddress.hpp
//!
namespace CB
{
    //!
    //! \class Address  Object to hold address information
    //!
    class Address
    {
        public:
            //!
            //! \brief Construct an address
            //!
            Address() : address(CB_DEFAULT_ADDRESS)
            {}

            //!
            //! \brief Construct an address
            //!
            //! \param address The uint32_t address
            //!
            Address(uint32_t address) : address(address)
            {}

            ~Address() = default;
            
            //!
            //! \brief Retrieve the address
            //!
            //! \returns address (uint32_t)
            //!
            uint32_t getAddress() const { return this->address; };

            //!
            //! \brief Operator == for Address object
            //!
            bool operator== (const Address &rhs)
            {
                if(this->address != rhs.address)
                {
                    return false;
                }
                return true;
            }

            //!
            //! \brief Operator = for Address object
            //!
            Address& operator= (const Address &rhs)
            {
                if(this != &rhs)
                {
                    this->address = rhs.address;
                }
                return *this;
            }

        private:
            uint32_t address;
            
    };

    //!
    //! \brief Typedef to define a list of addresses
    //!
    typedef std::list<Address> AddressList;
}

/*

    Operator for address that ensures its used in an std::map as
    a key, in its key-value store

*/
namespace std
{
    template<> struct less<CB::Address>
    {
       bool operator() (const CB::Address& lhs, const CB::Address& rhs) const
       {
           return lhs.getAddress() < rhs.getAddress();
       }
    };
}

#endif
