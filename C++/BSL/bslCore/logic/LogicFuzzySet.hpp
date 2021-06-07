#ifndef BSLCORE_FUZZY_SET_HPP
#define BSLCORE_FUZZY_SET_HPP

#include <map>
#include <string>
#include "LogicFuzzyBool.hpp"

//!
//! \file LogicFuzzySet.hpp
//! \brief Header for creating fuzzy sets
//!
namespace LOGIC
{
    //!
    //! \brief A FuzzySet
    //!
    class FuzzySet
    {
    public:
        //!
        //! \brief Construct a set
        //!
        FuzzySet();

        //!
        //! \brief Get the set data
        //! \returns Size of the set data 
        //!
        int size();

        //!
        //! \brief Check if the set is empty
        //! \returns true if empty, false otherwise
        //!
        bool isEmpty();

        //!
        //! \brief Erase an item in the map 
        //!
        void erase(std::string);

        //!
        //! \brief Empty the set data
        //!
        void clear();

        //!
        //! \brief Insert a boolean value
        //! \param key Data key
        //! \param value Data value
        //!
        void insert(std::string key, bool   value);

        //!
        //! \brief Insert a double value
        //! \param key Data key
        //! \param value Data value
        //!
        void insert(std::string key, double value);

        //!
        //! \brief Operator &&
        //! \param set Set to && current set with
        //! \retval &&'d fuzzy set
        //!
        FuzzySet operator&&(FuzzySet set);

        //!
        //! \brief Operator ||
        //! \param set Set to || current set with
        //! \retval ||'d fuzzy set
        //!
        FuzzySet operator||(FuzzySet set);

        //!
        //! \brief Operator !
        //! \retval Inverse of current set
        //!
        FuzzySet operator!();

        //!
        //! \brief Operator []
        //! \param key The data key to retrieve
        //! \returns Reference to value
        //!
        FuzzyBool& operator[](std::string key);

    private:
        std::map<std::string, FuzzyBool> setData;
    };

}
#endif // BSLCORE_FUZZY_SET_HPP
