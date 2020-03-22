#ifndef BUTANE_FUZZY_BOOLEAN
#define BUTANE_FUZZY_BOOLEAN

//!
//! \file UTILFuzzyBool.hpp
//! \brief Header for creating fuzzy booleans
//!
namespace BUTANE
{
    namespace UTIL
    {
        //!
        //! \brief A FuzzyBoolean
        //!
        class FuzzyBool
        {
        public:

            static constexpr double FUZZY_TRUE  = 1.0;  //! Fuzzy TRUE
            static constexpr double FUZZY_FALSE = 0.0;  //! Fuzzy FALSE

            //!
            //! \brief Create a fuzzy bool
            //!
            FuzzyBool();

            //!
            //! \brief Create a fuzzy bool from a standard bool
            //! \param value The value of the boolean
            //! \post The boolean will be FUZZY_TRUE or FUZZY_FALSE
            //!
            FuzzyBool(bool value);
            
            //!
            //! \brief Create a fuzzy bool from a double (chance of being true)
            //! \param value The probability of the boolean being true or false [0.0, +1.0]
            //!        value >= 1.0 -> Always true | value <= 0.0 => Always false
            //!
            FuzzyBool(double value);

            //!
            //! \brief Set to a new standard boolean value
            //! \param value The value of the boolean
            //! \post The boolean will be FUZZY_TRUE or FUZZY_FALSE
            //!
            void set(bool value);

            //!
            //! \brief Set to a new fuzzy boolean value
            //! \param value The probability of the boolean being true or false [0.0, +1.0]
            //!        value >= 1.0 -> Always true | value <= 0.0 => Always false
            //!
            void set(double value);

            //!
            //! \brief Get the fuzzy value of the boolean
            //! \returns A double depicting the chance of toBool returning true
            //!
            double get() const;

            //!
            //! \brief Get the boolean value of the fuzzy bool
            //! \returns Could return true or false depending on on the chances given
            //!          via set() or on construction. 
            //!
            bool toBool() const;

            //!
            //! \brief AND the current bool with a fuzzy bool
            //!
            FuzzyBool operator&&(FuzzyBool);

            //!
            //! \brief AND the current bool with a bool
            //!
            FuzzyBool operator&&(bool);

            //!
            //! \brief OR the current bool with a fuzzy bool
            //!
            FuzzyBool operator||(FuzzyBool);
            
            //!
            //! \brief OR the current bool with a bool
            //!
            FuzzyBool operator||(bool);
            
            //!
            //! \brief NOT the current bool
            //!
            FuzzyBool operator!();

        protected:
            static bool randomized;
            double probability;
        };
    }
}

#endif