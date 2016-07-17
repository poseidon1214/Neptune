/**
 *   Copyright (C) XXX. All rights reserved.

 *   \file     mod_json.hh
 *   \author   RainVan(Yunfeng.Xiao)
 *   \date     Dev 2012
 *   \version  1.0.0
 *   \brief    Interface of JSON Parser/Generator (C++)
 */

#ifndef __MOD_JSON_HH__
#define __MOD_JSON_HH__

#include <cfloat>
#include <cstring>
#include <string>

MODFRAME_NS_BEGIN

/*! JSON Exception
 */
class json_exception
{
public:
    enum { 
        unexpected_error = 1, 
        unmatched_type   = 2, 
        invalid_argument = 3, 
        out_of_range     = 4
    };

    //! Constructor
    json_exception(void) throw() : code_(0)
    {
    }

    //! Constructor
    json_exception(const json_exception &rhs) throw() : code_(rhs.code_)
    {
    }

    //! Constructor
    json_exception(size_type num) throw() : code_(num)
    {
    }

    //! Destructor
    ~json_exception(void) throw()
    {
    }

    //! Assign new contents to the exception, replacing its current content
    json_exception& operator=(const json_exception &rhs) throw()
    {
        code_ = rhs.code_;
        return *this;
    }

    //! Retrieve the code of exception 
    size_type code(void) const throw()
    {
        return code_;
    }

private:
    size_type code_;

};

/*! JSON String
 */
class json_string
{
public:
    //! Constructor
    json_string(void) : str_(0)
    {
    }

    //! Constructor
    json_string(const json_string &rhs) : str_(0)
    {
        if (rhs.str_)
        {
            str_ = mod_json_string_grab(rhs.str_);
        }
    }

    //! Constructor
    json_string(const char *cstr)
    {
        str_ = mod_json_string_set(cstr, cstr ? std::strlen(cstr) : 0);
    }

    //! Constructor
    json_string(const char *cstr, size_type len)
    {
        str_ = mod_json_string_set(cstr, len);
    }

    //! Constructor
    json_string(const std::string &str)
    {
        str_ = mod_json_string_set(str.c_str(), str.size());
    }

    //! Destructor
    ~json_string(void)
    {
        mod_json_string_unset(str_);
    }

    //! Assign new contents to the string, replacing its current content
    json_string &operator=(json_string rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    //! Assign new contents to the string, replacing its current content
    json_string &operator=(const char *cstr)
    {
        if (!assign(cstr))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the string, replacing its current content
    json_string &operator=(const std::string &rhs)
    {
        if (!assign(rhs))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Append a JSON string 
    json_string &operator+=(const json_string &str)
    {
        if (!append(str))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Append a c-style string 
    json_string &operator+=(const char *cstr)
    {
        if (!append(cstr))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Append a character to string 
    json_string &operator+=(char c)
    {
        if (!append(c))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Equality
    bool operator==(const json_string &rhs) const
    {
        return (mod_json_string_compare(str_, rhs.str_) == 0);
    }

    //! No equality
    bool operator!=(const json_string &rhs) const
    {
        return !(*this == rhs);
    }

    //! Retrieve the character at index n
    char &operator[](size_type n) 
    {
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *(str_->first + n);
    }

    //! Retrieve the character at index n
    const char &operator[](size_type n) const
    {
        return *(str_->first + n);
    }

    //! Retrieve non-zero if the string is valid
    bool is_valid(void) const
    {
        return (str_ != (mod_json_string_t*)0);
    }

    //! Retrieve non-zero if the string is empty
    bool empty(void) const
    {
        return (mod_json_string_empty(str_) == MOD_JSON_TRUE);
    }

    //! Assign a JSON string 
    void assign(json_string str)
    {
        str.swap(*this);
    }

    //! Assign a c-style string 
    bool assign(const char *cstr)
    {
        if (copy_on_write())
        {
            return (
                mod_json_string_assign(
                    str_, cstr, cstr ? std::strlen(cstr) : 0
                    ) == 0
                );
        }
        return false;
    }

    //! Assign a c-style string 
    bool assign(const char *cstr, size_type len)
    {
        if (copy_on_write())
        {
            return (mod_json_string_assign(str_, cstr, len) == 0);
        }
        return false;
    }

    //! Assign a STL-style string 
    bool assign(const std::string &str)
    {
        if (copy_on_write())
        {
            return (
                mod_json_string_assign(str_, str.c_str(), str.size()) == 0
                );
        }
        return false;
    }

    //! Append a JSON string 
    bool append(const json_string &str)
    {
        if (str.str_ && copy_on_write())
        {
            return (mod_json_string_add(str_, str.str_) == 0);
        }
        return false;
    }

    //! Append a c-style string 
    bool append(const char *cstr)
    {
        if (cstr && copy_on_write())
        {
            return (
                mod_json_string_append(str_, cstr, std::strlen(cstr)) == 0
                );
        }
        return false;
    }

    //! Append a c-style string 
    bool append(const char *cstr, size_type len)
    {
        if (cstr && copy_on_write())
        {
            return (mod_json_string_append(str_, cstr, len) == 0);
        }
        return false;
    }

    //! Append a STL-style string 
    bool append(const std::string &str)
    {
        if (copy_on_write())
        {
            return (
                mod_json_string_append(str_, str.c_str(), str.size()) == 0
                );
        }
        return false;
    }

    //! Append a character to string 
    bool append(char c)
    {
        if (copy_on_write())
        {
            return (mod_json_string_append(str_, &c, 1) == 0);
        }
        return false;
    }

    //! Retrieve the character at index n
    char &at(size_type n)
    {
        if (this->size() <= n)
        {
            throw json_exception(json_exception::out_of_range);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *(str_->first + n);
    }

    //! Retrieve the character at index n
    const char &at(size_type n) const
    {
        if (this->size() <= n)
        {
            throw json_exception(json_exception::out_of_range);
        }
        return *(str_->first + n);
    }

    //! Request a change in capacity
    bool reserve(size_type n)
    {
        if (copy_on_write())
        {
            return (mod_json_string_reserve(str_, n) == 0);
        }
        return false;
    }

    //! Clear the JSON string
    void clear(void)
    {
        mod_json_string_unset(str_);
        str_ = 0;
    }

    //! Exchange the content with another JSON string
    void swap(json_string &rhs)
    {
        mod_json_string_t *str = str_;
        str_ = rhs.str_;
        rhs.str_ = str;
    }

    //! Retrieve the data pointer 
    char *data(void)
    {
        return mod_json_string_data(str_);
    }

    //! Retrieve the data pointer 
    const char *data(void) const
    {
        return mod_json_string_data(str_);
    }

    //! Retrieve HASH of a JSON string
    size_type hash(void) const
    {
        return mod_json_string_hash(str_);
    }

    //! Compare two JSON strings (case sensitive)
    int compare(const json_string &rhs) const
    {
        return mod_json_string_compare(str_, rhs.str_);
    }

    //! Compare two strings (case sensitive)
    int compare(const char *cstr) const
    {
        const char *self = this->c_str();
        if (self && cstr)
        {
            return std::strcmp(self, cstr);
        }

        // particular case 
        if (!self && cstr)
        {
            return -1;
        }
        else if (self && !cstr)
        {
            return 1;
        }
        return 0;
    }

    // Encode a JSON string
    json_string encode(void) const
    {
        json_string ret;
        ret.str_ = mod_json_string_encode(str_);
        return ret;
    }

    // Decode a JSON string
    json_string decode(void) const
    {
        json_string ret;
        ret.str_ = mod_json_string_decode(str_);
        return ret;
    }

    //! Retrieve the capacity of string 
    size_type capacity(void) const
    {
        return mod_json_string_capacity(str_);
    }

    //! Retrieve the length of string 
    size_type size(void) const
    {
        return mod_json_string_length(str_);
    }

    //! Retrieve the length of string 
    size_type length(void) const
    {
        return mod_json_string_length(str_);
    }

    //! Retrieve refer-counter of string
    ssize_type refer(void) const
    {
        return mod_json_string_refer(str_);
    }

    //! Retrieve the c-style string 
    const char *c_str(void) const
    {
        return mod_json_string_cstr(str_);
    }

    //! Convert string to float
    float as_float(void) const
    {
        return (float)mod_json_string_float(str_);
    }

    //! Convert string to double
    double as_double(void) const
    {
        return (double)mod_json_string_float(str_);
    }

    //! Convert string to char integer
    char as_char(void) const
    {
        return (char)mod_json_string_integer(str_);
    }

    //! Convert string to short integer
    short as_short(void) const
    {
        return (short)mod_json_string_integer(str_);
    }

    //! Convert string to integer
    int as_int(void) const
    {
        return (int)mod_json_string_integer(str_);
    }

    //! Convert string to long integer
    long as_long(void) const
    {
        return (long)mod_json_string_integer(str_);
    }

    //! Convert string to long long integer
    long long as_llong(void) const
    {
        return (long long)mod_json_string_integer(str_);
    }

    //! Convert string to unsigned char integer
    unsigned char as_uchar(void) const
    {
        return (unsigned char)mod_json_string_integer(str_);
    }

    //! Convert string to unsigned short integer
    unsigned short as_ushort(void) const
    {
        return (unsigned short)mod_json_string_integer(str_);
    }

    //! Convert string to unsigned integer
    unsigned int as_uint(void) const
    {
        return (unsigned int)mod_json_string_integer(str_);
    }

    //! Convert string to unsigned long integer
    unsigned long as_ulong(void) const
    {
        return (unsigned long)mod_json_string_integer(str_);
    }

    //! Convert string to unsigned long long integer
    unsigned long long as_ullong(void) const
    {
        return (unsigned long long)mod_json_string_integer(str_);
    }

    //! Retrieve string as 8-bits integer
    int8_t as_int8(void) const
    {
        return (int8_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as 16-bits integer
    int16_t as_int16(void) const
    {
        return (int16_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as 32-bits integer
    int32_t as_int32(void) const
    {
        return (int32_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as 64-bits integer
    int64_t as_int64(void) const
    {
        return (int64_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as unsigned 8-bits integer
    uint8_t as_uint8(void) const
    {
        return (uint8_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as unsigned 16-bits integer
    uint16_t as_uint16(void) const
    {
        return (uint16_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as unsigned 32-bits integer
    uint32_t as_uint32(void) const
    {
        return (uint32_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as unsigned 64-bits integer
    uint64_t as_uint64(void) const
    {
        return (uint64_t)mod_json_string_integer(str_);
    }

    //! Retrieve string as a STL string
    std::string as_stlstring(void) const
    {
        if (!this->empty())
        {
            return std::string(this->data(), this->size());
        }
        return std::string();
    }

protected:
    //! Clone the string for writing 
    bool copy_on_write(void)
    {
        if (str_)
        {
            if (mod_json_string_is_shared(str_))
            {
                (void)mod_json_string_put(str_);
                str_ = mod_json_string_clone(str_);
            }
        }
        else
        {
            str_ = mod_json_string_set("", 0);
        }
        return (str_ != 0);
    }

    //! Clone the value and leak it 
    bool copy_and_leak(void)
    {
        if (copy_on_write())
        {
            mod_json_string_set_leaked(str_);
            return true;
        }
        return false;
    }

private:
    mod_json_string_t *str_;

};

class json_array;
class json_object;

/*! JSON Value
 */
class json_value
{
public:
    //! Constructor
    json_value(void) : val_(0)
    {
    }

    //! Constructor
    explicit json_value(const bool &val)
    {
        val_ = mod_json_value_set_boolean((mod_json_boolean_t)val);
    }

    //! Constructor
    explicit json_value(const char &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const short &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const int &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const long &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const long long &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const float &val)
    {
        val_ = mod_json_value_set_float((mod_json_float_t)val);
    }

    //! Constructor
    explicit json_value(const double &val)
    {
        val_ = mod_json_value_set_float((mod_json_float_t)val);
    }

    //! Constructor
    explicit json_value(const unsigned char &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const unsigned short &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const unsigned int &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const unsigned long &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    explicit json_value(const unsigned long long &val)
    {
        val_ = mod_json_value_set_integer((mod_json_integer_t)val);
    }

    //! Constructor
    json_value(const json_string &val)
    {
        val_ = mod_json_value_set_string(*(mod_json_string_t**)&val);
    }

    //! Constructor
    json_value(const char *val)
    {
        json_string str(val);
        val_ = mod_json_value_set_string(*(mod_json_string_t**)&str);
    }

    //! Constructor
    json_value(const char *val, size_type len)
    {
        json_string str(val, len);
        val_ = mod_json_value_set_string(*(mod_json_string_t**)&str);
    }

    //! Constructor
    json_value(const std::string &val)
    {
        json_string str(val);
        val_ = mod_json_value_set_string(*(mod_json_string_t**)&str);
    }

    //! Constructor
    json_value(const json_array &val)
    {
        val_ = mod_json_value_set_array(*(mod_json_array_t**)&val);
    }

    //! Constructor
    json_value(const json_object &val)
    {
        val_ = mod_json_value_set_object(*(mod_json_object_t**)&val);
    }

    //! Constructor
    json_value(const json_value &rhs) : val_(0)
    {
        if (rhs.val_)
        {
            val_ = mod_json_value_grab(rhs.val_);
        }
    }

    //! Destructor
    ~json_value(void)
    {
        mod_json_value_unset(val_);
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(json_value rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const bool &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const char &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const short &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const int &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const long &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const long long &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const float &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const double &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const unsigned char &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const unsigned short &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const unsigned int &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const unsigned long &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const unsigned long long &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const json_string &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const char *val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(const std::string &val)
    {
        if (!assign(val))
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return *this;
    }

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(json_array arr);

    //! Assign new contents to the value, replacing its current content
    json_value &operator=(json_object obj);

    //! Equality
    bool operator==(const json_value &rhs) const
    {
        return (mod_json_value_is_equal(val_, rhs.val_) == MOD_JSON_TRUE);
    }

    //! No equality
    bool operator!=(const json_value &rhs) const
    {
        return !(*this == rhs);
    }

    //! Treat self value as object by force, retrieving value of a key
    json_value &operator[](const char *key)
    {
        return get_value(key);
    }

    //! Retrieve a reference of value by a key
    const json_value &operator[](const char *key) const
    {
        return get_value(key);
    }

    //! Treat self value as object by force, retrieving value of a key
    json_value &operator[](const json_string &key)
    {
        return get_value(key.c_str());
    }

    //! Retrieve a reference of value by a key
    const json_value &operator[](const json_string &key) const
    {
        return get_value(key.c_str());
    }

    //! Treat self value as array by force, retrieving value at index n
    json_value &operator[](size_type n)
    {
        return get_value(n);
    }

    //! Retrieve a reference of value at index n
    const json_value &operator[](size_type n) const
    {
        return get_value(n);
    }

    //! Retrieve non-zero if the value is valid
    bool is_valid(void) const
    {
        return (val_ != (mod_json_value_t*)0);
    }

    //! Retrieve non-zero if the value is a object
    bool is_object(void) const
    {
        return (mod_json_value_is_object(val_) == MOD_JSON_TRUE);
    }

    //! Retrieve non-zero if the value is an array
    bool is_array(void) const
    {
        return (mod_json_value_is_array(val_) == MOD_JSON_TRUE);
    }

    //! Retrieve non-zero if the value is a string
    bool is_string(void) const
    {
        return (mod_json_value_is_string(val_) == MOD_JSON_TRUE);
    }

    //! Retrieve non-zero if the value is null
    bool is_null(void) const
    {
        return (mod_json_value_is_null(val_) == MOD_JSON_TRUE);
    }

    //! Retrieve non-zero if the value is a float
    bool is_float(void) const
    {
        return (mod_json_value_is_float(val_) == MOD_JSON_TRUE);
    }

    //! Retrieve non-zero if the value is an integer
    bool is_integer(void) const
    {
        return (mod_json_value_is_integer(val_) == MOD_JSON_TRUE);
    }

    //! Retrieve non-zero if the value is a boolean
    bool is_boolean(void) const
    {
        return (mod_json_value_is_boolean(val_) == MOD_JSON_TRUE);
    }

    //! Assign new contents to the value, replacing its current content
    void assign(json_value val)
    {
        val.swap(*this);
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const bool &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_boolean(val_, (mod_json_boolean_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const char &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const short &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const int &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const long &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const long long &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const float &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_float(val_, (mod_json_float_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const double &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_float(val_, (mod_json_float_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const unsigned char &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const unsigned short &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const unsigned int &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const unsigned long &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const unsigned long long &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_integer(val_, (mod_json_integer_t)val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const json_string &val)
    {
        if (copy_on_write())
        {
            mod_json_value_assign_string(val_, *(mod_json_string_t**)&val);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const char *val)
    {
        json_string str(val);
        if (str.is_valid() && copy_on_write())
        {
            mod_json_value_assign_string(val_, *(mod_json_string_t**)&str);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const char *val, size_type len)
    {
        json_string str(val, len);
        if (str.is_valid() && copy_on_write())
        {
            mod_json_value_assign_string(val_, *(mod_json_string_t**)&str);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(const std::string &val)
    {
        json_string str(val);
        if (str.is_valid() && copy_on_write())
        {
            mod_json_value_assign_string(val_, *(mod_json_string_t**)&str);
            return true;
        }
        return false;
    }

    //! Assign new contents to the value, replacing its current content
    bool assign(json_array arr);

    //! Assign new contents to the value, replacing its current content
    bool assign(json_object obj);

    //! Retrieve refer-counter of JSON value
    ssize_type refer(void) const
    {
        return mod_json_value_refer(val_);
    }

    //! Retrieve value as JSON format string
    json_string as_jsonstring(void) const
    {
        mod_json_string_t *tmp = mod_json_dump(val_);
        json_string ret = *reinterpret_cast<json_string*>(&tmp);
        if (tmp)
        {
            mod_json_string_unset(tmp);
        }
        return ret;
    }

    //! Retrieve value as a STL string
    std::string as_stlstring(void) const
    {
        if (is_string())
        {
            return to_string().as_stlstring();
        }
        return std::string();
    }

    //! Retrieve value as JSON string
    const json_string &as_string(void) const
    {
        if (!is_string())
        {
            throw json_exception(json_exception::unmatched_type);
        }
        return to_string();
    }

    //! Retrieve value as c-style string
    const char *as_cstring(void) const
    {
        return mod_json_value_cstring(val_);
    }

    //! Retrieve value as JSON string
    json_string &as_string(void)
    {
        if (!is_string())
        {
            throw json_exception(json_exception::unmatched_type);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return to_string();
    }

    //! Retrieve value as JSON array
    const json_array &as_array(void) const
    {
        if (!is_array())
        {
            throw json_exception(json_exception::unmatched_type);
        }
        return to_array();
    }

    //! Retrieve value as JSON array
    json_array &as_array(void)
    {
        if (!is_array())
        {
            throw json_exception(json_exception::unmatched_type);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return to_array();
    }

    //! Retrieve value as JSON object
    const json_object &as_object(void) const
    {
        if (!is_object())
        {
            throw json_exception(json_exception::unmatched_type);
        }
        return to_object();
    }

    //! Retrieve value as JSON object
    json_object &as_object(void)
    {
        if (!is_object())
        {
            throw json_exception(json_exception::unmatched_type);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return to_object();
    }

    //! Retrieve value as float
    float as_float(void) const
    {
        return (float)mod_json_value_float(val_);
    }

    //! Retrieve value as double
    double as_double(void) const
    {
        return (double)mod_json_value_float(val_);
    }

    //! Retrieve value as char integer
    char as_char(void) const
    {
        return (char)mod_json_value_integer(val_);
    }

    //! Retrieve value as short integer
    short as_short(void) const
    {
        return (short)mod_json_value_integer(val_);
    }

    //! Retrieve value as integer
    int as_int(void) const
    {
        return (int)mod_json_value_integer(val_);
    }

    //! Retrieve value as long integer
    long as_long(void) const
    {
        return (long)mod_json_value_integer(val_);
    }

    //! Retrieve value as long long integer
    long long as_llong(void) const
    {
        return (long long)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned char integer
    unsigned char as_uchar(void) const
    {
        return (unsigned char)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned short integer
    unsigned short as_ushort(void) const
    {
        return (unsigned short)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned integer
    unsigned int as_uint(void) const
    {
        return (unsigned int)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned long integer
    unsigned long as_ulong(void) const
    {
        return (unsigned long)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned long long integer
    unsigned long long as_ullong(void) const
    {
        return (unsigned long long)mod_json_value_integer(val_);
    }

    //! Retrieve value as 8-bits integer
    int8_t as_int8(void) const
    {
        return (int8_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as 16-bits integer
    int16_t as_int16(void) const
    {
        return (int16_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as 32-bits integer
    int32_t as_int32(void) const
    {
        return (int32_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as 64-bits integer
    int64_t as_int64(void) const
    {
        return (int64_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned 8-bits integer
    uint8_t as_uint8(void) const
    {
        return (uint8_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned 16-bits integer
    uint16_t as_uint16(void) const
    {
        return (uint16_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned 32-bits integer
    uint32_t as_uint32(void) const
    {
        return (uint32_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as unsigned 64-bits integer
    uint64_t as_uint64(void) const
    {
        return (uint64_t)mod_json_value_integer(val_);
    }

    //! Retrieve value as boolean
    bool as_bool(void) const
    {
        return (mod_json_value_boolean(val_) == MOD_JSON_TRUE);
    }

    //! Exchange the content with another JSON value
    void swap(json_value &rhs)
    {
        mod_json_value_t *val = val_;
        val_ = rhs.val_;
        rhs.val_ = val;
    }

    //! Merge another JSON value 
    bool merge(const json_value &rhs)
    {
        if (copy_on_write())
        {
            return (mod_json_value_merge(val_, rhs.val_) == 0);
        }
        return false;
    }

    //! Parse a sting as a JSON value 
    bool parse(const char *str)
    {
        mod_json_token_t *tok = mod_json_token_create(NULL);

        if (tok)
        {
            mod_json_value_t *jval = mod_json_parse(tok, str);

            mod_json_token_destroy(tok);
            if (jval)
            {
                *this = *reinterpret_cast<json_value*>(&jval);
                mod_json_value_unset(jval);
                return is_valid();
            }
        }
        return false;
    }

    //! Parse a sting as a JSON value 
    bool parse(const json_string &str)
    {
        return this->parse(str.c_str());
    }

    //! Parse a sting as a JSON value 
    bool parse(const std::string &str)
    {
        return this->parse(str.c_str());
    }

    //! Retrieve reference of a invalid JSON value 
    static const json_value &invalid(void)
    {
        return _invalid;
    }

protected:
    //! Clone the value for writing 
    bool copy_on_write(void)
    {
        if (val_)
        {
            if (mod_json_value_is_shared(val_))
            {
                (void)mod_json_value_put(val_);
                val_ = mod_json_value_clone(val_);
            }
        }
        else
        {
            val_ = mod_json_value_set_null();
        }
        return (val_ != 0);
    }

    //! Clone the value and leak it 
    bool copy_and_leak(void)
    {
        if (copy_on_write())
        {
            mod_json_value_set_leaked(val_);
            return true;
        }
        return false;
    }

    //! Convert value to JSON object
    json_object &to_object(void);

    //! Convert value to JSON object
    const json_object &to_object(void) const;

    //! Convert value to JSON array
    json_array &to_array(void);

    //! Convert value to JSON array
    const json_array &to_array(void) const;

    //! Convert value to JSON string
    json_string &to_string(void);

    //! Convert value to JSON string
    const json_string &to_string(void) const;

    //! Treat self value as object by force, retrieving value of a key
    json_value &get_value(const char *key);

    //! Retrieve a reference of value by a key
    const json_value &get_value(const char *key) const;

    //! Treat self value as array by force, retrieving value at index n
    json_value &get_value(size_type n);

    //! Retrieve a reference of value at index n
    const json_value &get_value(size_type n) const;

    //! Set the new array to the value, replacing its current content
    bool set_value(const json_array &val);

    //! Set the new object to the value, replacing its current content
    bool set_value(const json_object &val);

protected:
    static const json_value _invalid;

private:
    mod_json_value_t *val_;

};

/*! JSON Array
 */
class json_array
{
public:

    class iterator;

    /*! Const Iterator of JSON Array
     */
    class const_iterator
    {
        friend class json_array;
        friend class json_array::iterator;

    public:
        //! Constructor
        const_iterator(void) : iter_(0)
        {
        }

        //! Equality
        bool operator==(const const_iterator &rhs) const
        {
            return (iter_ == rhs.iter_);
        }

        //! No equality
        bool operator!=(const const_iterator &rhs) const
        {
            return (iter_ != rhs.iter_);
        }

        //! Increment (Prefix)
        const_iterator &operator++()
        {
            ++iter_;
            return *this;
        }

        //! Increment (Suffix)
        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++iter_;
            return tmp;
        }

        //! Decrement (Prefix)
        const_iterator &operator--()
        {
            --iter_;
            return *this;
        }

        //! Decrement (Suffix)
        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            --iter_;
            return tmp;
        }

        //! Indirection (eg. *iter)
        const json_value &operator*() const
        {
            return *reinterpret_cast<const json_value*>(iter_);
        }

        //! Structure dereference (eg. iter->)
        const json_value *operator->() const
        {
            return reinterpret_cast<const json_value*>(iter_);
        }

    protected:
        //! Constructor for friends
        const_iterator(mod_json_value_t *const *iter) : iter_(iter)
        {
        }

    private:
        mod_json_value_t *const *iter_;
    };

    /*! Iterator of JSON Array
     */
    class iterator
    {
        friend class json_array;

    public:
        //! Constructor
        iterator(void) : iter_(0)
        {
        }

        //! Equality
        bool operator==(const iterator &rhs) const
        {
            return (iter_ == rhs.iter_);
        }

        //! No equality
        bool operator!=(const iterator &rhs) const
        {
            return (iter_ != rhs.iter_);
        }

        //! Increment (Prefix)
        iterator &operator++()
        {
            ++iter_;
            return *this;
        }

        //! Increment (Suffix)
        iterator operator++(int)
        {
            iterator tmp = *this;
            ++iter_;
            return tmp;
        }

        //! Decrement (Prefix)
        iterator &operator--()
        {
            --iter_;
            return *this;
        }

        //! Decrement (Suffix)
        iterator operator--(int)
        {
            iterator tmp = *this;
            --iter_;
            return tmp;
        }

        //! Indirection (eg. *iter)
        json_value &operator*() const 
        {
            return *reinterpret_cast<json_value*>(iter_);
        }

        //! Structure dereference (eg. iter->)
        json_value *operator->() const
        {
            return reinterpret_cast<json_value*>(iter_);
        }

        //! Retrieve as const iterator
        operator const_iterator() const
        {
            return const_iterator(iter_);
        }

    protected:
        //! Constructor for friends
        iterator(mod_json_value_t **iter) : iter_(iter)
        {
        }

    private:
        mod_json_value_t **iter_;
    };

    //! Constructor
    json_array(void) : arr_(0)
    {
    }

    //! Constructor
    json_array(const json_array &rhs) : arr_(0)
    {
        if (rhs.arr_)
        {
            arr_ = mod_json_array_grab(rhs.arr_);
        }
    }

    //! Destructor
    ~json_array(void)
    {
        mod_json_array_unset(arr_);
    }

    //! Assign new contents to the array, replacing its current content
    json_array &operator=(json_array rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    //! Equality
    bool operator==(const json_array &rhs) const
    {
        return (mod_json_array_is_equal(arr_, rhs.arr_) == MOD_JSON_TRUE);
    }

    //! No equality
    bool operator!=(const json_array &rhs) const
    {
        return !(*this == rhs);
    }

    //! Retrieve the value at index n, if no one exists, throw an exception.
    json_value &operator[](size_type n) 
    {
        return this->at(n);
    }

    //! Retrieve the value at index n, if no one exists, return a null value.
    const json_value &operator[](size_type n) const
    {
        return ((n < this->size()) ? get_value(n) : json_value::invalid());
    }

    //! Retrieve non-zero if the array is valid
    bool is_valid(void) const
    {
        return (arr_ != (mod_json_array_t*)0);
    }

    //! Retrieve non-zero if the array is empty
    bool empty(void) const
    {
        return (mod_json_array_empty(arr_) == MOD_JSON_TRUE);
    }

    //! Retrieve the size of JSON array
    size_type size(void) const
    {
        return mod_json_array_count(arr_);
    }

    //! Retrieve the capacity of JSON array
    size_type capacity(void) const
    {
        return mod_json_array_capacity(arr_);
    }

    //! Retrieve refer-counter of JSON array
    ssize_type refer(void) const
    {
        return mod_json_array_refer(arr_);
    }

    //! Assign new contents to the array, replacing its current content
    void assign(json_array arr)
    {
        arr.swap(*this);
    }

    //! Request a change in capacity
    bool reserve(size_type n)
    {
        if (copy_on_write())
        {
            return (mod_json_array_reserve(arr_, n) == 0);
        }
        return false;
    }

    //! Push a value to array
    bool push(const json_value &val)
    {
        json_value tmp(val);

        if (copy_on_write())
        {
            return (
                mod_json_array_push(
                arr_, *((mod_json_value_t**)&tmp)
                ) == 0
                );
        }
        return false;
    }

    //! Pop the last element from array
    void pop(void)
    {
        if (arr_ && copy_on_write())
        {
            mod_json_array_pop(arr_);
        }
    }

    //! Append a value to array
    bool append(const json_value &val)
    {
        return push(val);
    }

    //! Retrieve the value at index n
    json_value &at(size_type n)
    {
        if (this->size() <= n)
        {
            throw json_exception(json_exception::out_of_range);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return get_value(n);
    }

    //! Retrieve the value at index n
    const json_value &at(size_type n) const
    {
        if (this->size() <= n)
        {
            throw json_exception(json_exception::out_of_range);
        }
        return get_value(n);
    }

    //! Retrieve a reference to the first element
    json_value &front(void)
    {
        if (this->size() <= 0)
        {
            throw json_exception(json_exception::out_of_range);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return get_value(0);
    }

    //! Retrieve a reference to the first element
    const json_value &front(void) const
    {
        if (this->size() <= 0)
        {
            throw json_exception(json_exception::out_of_range);
        }
        return get_value(0);
    }

    //! Retrieve a reference to the last element
    json_value &back(void)
    {
        if (this->size() <= 0)
        {
            throw json_exception(json_exception::out_of_range);
        }
        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return get_value(this->size() - 1);
    }

    //! Retrieve a reference to the last element
    const json_value &back(void) const
    {
        if (this->size() <= 0)
        {
            throw json_exception(json_exception::out_of_range);
        }
        return get_value(this->size() - 1);
    }

    //! Clear the JSON array
    void clear(void)
    {
        mod_json_array_unset(arr_);
        arr_ = 0;
    }

    //! Exchange the content with another JSON array
    void swap(json_array &rhs)
    {
        mod_json_array_t *arr = arr_;
        arr_ = rhs.arr_;
        rhs.arr_ = arr;
    }

    //! Merge another JSON array 
    bool merge(const json_array &rhs)
    {
        if (copy_on_write())
        {
            return (mod_json_array_merge(arr_, rhs.arr_) == 0);
        }
        return false;
    }

    //! Resize a JSON array so that it contains n elements
    bool resize(size_type n, const json_value &val = json_value())
    {
        if (copy_on_write())
        {
            return (
                mod_json_array_resize(
                    arr_, n, *((mod_json_value_t**)&val)
                    ) == 0
                );
        }
        return false;
    }

    //! Retrieve an iterator pointing to the first element
    iterator begin(void)
    {
        if (copy_and_leak())
        {
             return iterator(mod_json_array_begin(arr_));
        }
        return iterator();
    }

    //! Retrieve a const iterator pointing to the first element
    const_iterator begin(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_begin(arr_));
        }
        return const_iterator();
    }

    //! Retrieve a const iterator pointing to the first element
    const_iterator cbegin(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_begin(arr_));
        }
        return const_iterator();
    }

    //! Retrieve a reverse iterator pointing to the last element
    iterator rbegin(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_array_rbegin(arr_));
        }
        return iterator();
    }

    //! Retrieve a const reverse iterator pointing to the last element
    const_iterator rbegin(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_rbegin(arr_));
        }
        return const_iterator();
    }

    //! Retrieve a const reverse iterator pointing to the last element
    const_iterator crbegin(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_rbegin(arr_));
        }
        return const_iterator();
    }

    //! Retrieve an iterator pointing to the past-the-end element
    iterator end(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_array_end(arr_));
        }
        return iterator();
    }

    //! Retrieve a const iterator pointing to the past-the-end element
    const_iterator end(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_end(arr_));
        }
        return const_iterator();
    }

    //! Retrieve a const iterator pointing to the past-the-end element
    const_iterator cend(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_end(arr_));
        }
        return const_iterator();
    }

    //! Retrieve a reverse pointing to the past-the-end element
    iterator rend(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_array_rend(arr_));
        }
        return iterator();
    }

    //! Retrieve a const reverse pointing to the past-the-end element
    const_iterator rend(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_rend(arr_));
        }
        return const_iterator();
    }

    //! Retrieve a const reverse pointing to the past-the-end element
    const_iterator crend(void) const
    {
        if (arr_)
        {
            return const_iterator(mod_json_array_rend(arr_));
        }
        return const_iterator();
    }

protected:
    //! Clone the array for writing 
    bool copy_on_write(void)
    {
        if (arr_)
        {
            if (mod_json_array_is_shared(arr_))
            {
                (void)mod_json_array_put(arr_);
                arr_ = mod_json_array_clone(arr_);
            }
        }
        else
        {
            arr_ = mod_json_array_set(0);
        }
        return (arr_ != 0);
    }

    //! Clone the array and leak it 
    bool copy_and_leak(void)
    {
        if (copy_on_write())
        {
            mod_json_array_set_leaked(arr_);
            return true;
        }
        return false;
    }

    //! Retrieve the value at index n
    json_value &get_value(size_type n)
    {
        return *reinterpret_cast<json_value*>(arr_->first + n);
    }

    //! Retrieve the value at index n
    const json_value &get_value(size_type n) const
    {
        return *reinterpret_cast<json_value*>(arr_->first + n);
    }

private:
    mod_json_array_t *arr_;

};

/*! JSON Pair
 */
class json_pair
{
    friend class json_object;

public:
    //! Constructor
    json_pair(void) : pair_(0)
    {
    }

    //! Retrieve non-zero if the pair is valid
    bool is_valid(void) const
    {
        return (pair_ != (mod_json_pair_t*)0);
    }

    //! Retrieve the key of pair 
    const json_string &key(void) const
    {
        return *reinterpret_cast<json_string*>(&pair_->key);
    }

    //! Retrieve the value of pair 
    json_value &value(void)
    {
        return *reinterpret_cast<json_value*>(&pair_->val);
    }

    //! Retrieve the value of pair 
    const json_value &value(void) const
    {
        return *reinterpret_cast<json_value*>(&pair_->val);
    }

protected:
    //! Constructor for friends
    json_pair(mod_json_pair_t *pair) : pair_(pair)
    {
    }

private:
    mod_json_pair_t *pair_;

};

/*! JSON Object
 */
class json_object
{
public:

    class iterator;

    /*! Const Iterator of JSON Object
     */
    class const_iterator
    {
        friend class json_object;
        friend class json_object::iterator;

    public:
        //! Constructor
        const_iterator(void) : iter_(0)
        {
        }

        //! Equality
        bool operator==(const const_iterator &rhs) const
        {
            return (iter_ == rhs.iter_);
        }

        //! No equality
        bool operator!=(const const_iterator &rhs) const
        {
            return (iter_ != rhs.iter_);
        }

        //! Increment (Prefix)
        const_iterator &operator++()
        {
            iter_ = mod_json_pair_next((mod_json_pair_t*)iter_);
            return *this;
        }

        //! Increment (Suffix)
        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            iter_ = mod_json_pair_next((mod_json_pair_t*)iter_);
            return tmp;
        }

        //! Decrement (Prefix)
        const_iterator &operator--()
        {
            iter_ = mod_json_pair_prev((mod_json_pair_t*)iter_);
            return *this;
        }

        //! Decrement (Suffix)
        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            iter_ = mod_json_pair_prev((mod_json_pair_t*)iter_);
            return tmp;
        }

        //! Indirection (eg. *iter)
        const json_pair &operator*() const
        {
            return *reinterpret_cast<const json_pair*>(&iter_);
        }

        //! Structure dereference (eg. iter->)
        const json_pair *operator->() const
        {
            return reinterpret_cast<const json_pair*>(&iter_);
        }

    protected:
        //! Constructor for friends
        const_iterator(const mod_json_pair_t *iter) : iter_(iter)
        {
        }

    private:
        const mod_json_pair_t *iter_;
    };

    /*! Iterator of JSON Object
     */
    class iterator
    {
        friend class json_object;

    public:
        //! Constructor
        iterator(void) : iter_(0)
        {
        }

        //! Equality
        bool operator==(const iterator &rhs) const
        {
            return (iter_ == rhs.iter_);
        }

        //! No equality
        bool operator!=(const iterator &rhs) const
        {
            return (iter_ != rhs.iter_);
        }

        //! Increment (Prefix)
        iterator &operator++()
        {
            iter_ = mod_json_pair_next(iter_);
            return *this;
        }

        //! Increment (Suffix)
        iterator operator++(int)
        {
            iterator tmp = *this;
            iter_ = mod_json_pair_next(iter_);
            return tmp;
        }

        //! Decrement (Prefix)
        iterator &operator--()
        {
            iter_ = mod_json_pair_prev(iter_);
            return *this;
        }

        //! Decrement (Suffix)
        iterator operator--(int)
        {
            iterator tmp = *this;
            iter_ = mod_json_pair_prev(iter_);
            return tmp;
        }

        //! Indirection (eg. *iter)
        json_pair &operator*() const
        {
            return *reinterpret_cast<json_pair*>((mod_json_pair_t**)&iter_);
        }

        //! Structure dereference (eg. iter->)
        json_pair *operator->() const
        {
            return reinterpret_cast<json_pair*>((mod_json_pair_t**)&iter_);
        }

        //! Retrieve as const iterator
        operator const_iterator() const
        {
            return const_iterator(iter_);
        }

    protected:
        //! Constructor for friends
        iterator(mod_json_pair_t *iter) : iter_(iter)
        {
        }

    private:
        mod_json_pair_t *iter_;
    };

    //! Constructor
    json_object(void) : obj_(0)
    {
    }

    //! Constructor
    json_object(const json_object &rhs) : obj_(0)
    {
        if (rhs.obj_)
        {
            obj_ = mod_json_object_grab(rhs.obj_);
        }
    }

    //! Destructor
    ~json_object(void)
    {
        mod_json_object_unset(obj_);
    }

    //! Assign new contents to the object, replacing its current content
    json_object &operator=(json_object rhs)
    {
        rhs.swap(*this);
        return *this;
    }

    //! Equality
    bool operator==(const json_object &rhs) const
    {
        return (mod_json_object_is_equal(obj_, rhs.obj_) == MOD_JSON_TRUE);
    }

    //! No equality
    bool operator!=(const json_object &rhs) const
    {
        return !(*this == rhs);
    }

    //! Retrieve the value of a key, if no one exists, create a new one.
    json_value &operator[](const char *key)
    {
        if (!key)
        {
            throw json_exception(json_exception::invalid_argument);
        }

        if (!copy_and_leak())
        {
            throw json_exception(json_exception::unexpected_error);
        }

        json_pair pair(mod_json_object_touch(obj_, key));
        if (!pair.is_valid())
        {
            throw json_exception(json_exception::unexpected_error);
        }
        return pair.value();
    }

    //! Retrieve the value of a key, if no one exists, return a null value.
    const json_value &operator[](const char *key) const
    {
        if (!key)
        {
            throw json_exception(json_exception::invalid_argument);
        }

        json_pair pair(mod_json_object_find(obj_, key));
        return (pair.is_valid() ? pair.value() : json_value::invalid());
    }

    //! Retrieve the value of a key, if no one exists, create a new one.
    json_value &operator[](const json_string &key)
    {
        return (*this)[key.c_str()];
    }

    //! Retrieve the value of a key, if no one exists, return a null value.
    const json_value &operator[](const json_string &key) const
    {
        return (*this)[key.c_str()];
    }

    //! Retrieve non-zero if the object is valid
    bool is_valid(void) const
    {
        return (obj_ != (mod_json_object_t*)0);
    }

    //! Retrieve non-zero if the object is empty
    bool empty(void) const
    {
        return (mod_json_object_empty(obj_) == MOD_JSON_TRUE);
    }

    //! Retrieve the size of JSON object
    size_type size(void) const
    {
        return mod_json_object_count(obj_);
    }

    //! Retrieve refer-counter of JSON object
    ssize_type refer(void) const
    {
        return mod_json_object_refer(obj_);
    }

    //! Assign new contents to the object, replacing its current content
    void assign(json_object obj)
    {
        obj.swap(*this);
    }

    //! Clear the JSON object
    void clear(void)
    {
        mod_json_object_unset(obj_);
        obj_ = 0;
    }

    //! Set the value of a key 
    bool set(const char *key, const json_value &val)
    {
        return this->set(json_string(key), val);
    }

    //! Set the value of a key 
    bool set(const json_string &key, const json_value &val)
    {
        json_value tmp(val);

        if (copy_on_write())
        {
            return (
                mod_json_object_insert(obj_, 
                *(mod_json_string_t**)&key, *(mod_json_value_t**)&tmp
                ) != (mod_json_pair_t*)0
                );
        }
        return false;
    }

    //! Retrieve the value of a key 
    json_value &get(const char *key, json_value &def)
    {
        if (copy_and_leak())
        {
            json_pair pair(mod_json_object_find(obj_, key));

            if (pair.is_valid())
            {
                return pair.value();
            }
        }
        return def;
    }

    //! Retrieve the value of a key 
    json_value &get(const json_string &key, json_value &def)
    {
        return this->get(key.c_str(), def);
    }

    //! Retrieve the value of a key 
    const json_value &get(const char *key, const json_value &def) const
    {
        const json_pair pair(mod_json_object_find(obj_, key));
        return (pair.is_valid() ? pair.value() : def);
    }

    //! Retrieve the value of a key 
    const json_value &get(const json_string &key, const json_value &def) const
    {
        return this->get(key.c_str(), def);
    }

    //! Delete a key-value pair from JSON object
    void unset(const char *key)
    {
        if (copy_on_write())
        {
            mod_json_object_erase(obj_, key);
        }
    }

    //! Retrieve non-zero if the key exists in JSON object
    bool has_key(const char *key) const
    {
        return (mod_json_object_find(obj_, key) != (mod_json_pair_t*)0);
    }

    //! Exchange the content with another JSON object
    void swap(json_object &rhs)
    {
        mod_json_object_t *obj = obj_;
        obj_ = rhs.obj_;
        rhs.obj_ = obj;
    }

    //! Merge another JSON object 
    bool merge(const json_object &rhs)
    {
        if (copy_on_write())
        {
            return (mod_json_object_merge(obj_, rhs.obj_) == 0);
        }
        return false;
    }

    //! Retrieve an iterator pointing to the first element
    iterator begin(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_object_begin(obj_));
        }
        return iterator();
    }

    //! Retrieve a const iterator pointing to the first element
    const_iterator begin(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_begin(obj_));
        }
        return const_iterator();
    }

    //! Retrieve a const iterator pointing to the first element
    const_iterator cbegin(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_begin(obj_));
        }
        return const_iterator();
    }

    //! Retrieve a reverse iterator pointing to the last element
    iterator rbegin(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_object_rbegin(obj_));
        }
        return iterator();
    }

    //! Retrieve a const reverse iterator pointing to the last element
    const_iterator rbegin(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_rbegin(obj_));
        }
        return const_iterator();
    }

    //! Retrieve a const reverse iterator pointing to the last element
    const_iterator crbegin(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_rbegin(obj_));
        }
        return const_iterator();
    }

    //! Retrieve an iterator pointing to the past-the-end element
    iterator end(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_object_end(obj_));
        }
        return iterator();
    }

    //! Retrieve a const iterator pointing to the past-the-end element
    const_iterator end(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_end(obj_));
        }
        return const_iterator();
    }

    //! Retrieve a const iterator pointing to the past-the-end element
    const_iterator cend(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_end(obj_));
        }
        return const_iterator();
    }

    //! Retrieve a reverse pointing to the past-the-end element
    iterator rend(void)
    {
        if (copy_and_leak())
        {
            return iterator(mod_json_object_rend(obj_));
        }
        return iterator();
    }

    //! Retrieve a const reverse pointing to the past-the-end element
    const_iterator rend(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_rend(obj_));
        }
        return const_iterator();
    }

    //! Retrieve a const reverse pointing to the past-the-end element
    const_iterator crend(void) const
    {
        if (obj_)
        {
            return const_iterator(mod_json_object_rend(obj_));
        }
        return const_iterator();
    }

protected:
    //! Clone the object for writing 
    bool copy_on_write(void)
    {
        if (obj_)
        {
            if (mod_json_object_is_shared(obj_))
            {
                (void)mod_json_object_put(obj_);
                obj_ = mod_json_object_clone(obj_);
            }
        }
        else
        {
            obj_ = mod_json_object_set(0);
        }
        return (obj_ != 0);
    }

    //! Clone the object and leak it 
    bool copy_and_leak(void)
    {
        if (copy_on_write())
        {
            mod_json_object_set_leaked(obj_);
            return true;
        }
        return false;
    }

private:
    mod_json_object_t *obj_;

};

//! Assign new contents to the value, replacing its current content
inline json_value &json_value::operator=(json_array arr)
{
    if (!set_value(arr))
    {
        throw json_exception(json_exception::unexpected_error);
    }
    return *this;
}

//! Assign new contents to the value, replacing its current content
inline json_value &json_value::operator=(json_object obj)
{
    if (!set_value(obj))
    {
        throw json_exception(json_exception::unexpected_error);
    }
    return *this;
}

//! Assign new contents to the value, replacing its current content
inline bool json_value::assign(json_array arr)
{
    return set_value(arr);
}

//! Assign new contents to the value, replacing its current content
inline bool json_value::assign(json_object obj)
{
    return set_value(obj);
}

//! Convert value to JSON object
inline json_object &json_value::to_object(void)
{
    return *reinterpret_cast<json_object*>(&val_->data.c_obj);
}

//! Convert value to JSON object
inline const json_object &json_value::to_object(void) const
{
    return *reinterpret_cast<json_object*>(&val_->data.c_obj);
}

//! Convert value to JSON array
inline json_array &json_value::to_array(void)
{
    return *reinterpret_cast<json_array*>(&val_->data.c_arr);
}

//! Convert value to JSON array
inline const json_array &json_value::to_array(void) const
{
    return *reinterpret_cast<json_array*>(&val_->data.c_arr);
}

//! Convert value to JSON string
inline json_string &json_value::to_string(void)
{
    return *reinterpret_cast<json_string*>(&val_->data.c_str);
}

//! Convert value to JSON string
inline const json_string &json_value::to_string(void) const
{
    return *reinterpret_cast<json_string*>(&val_->data.c_str);
}

//! Treat self value as object by force, retrieving value of a key
inline json_value &json_value::get_value(const char *key)
{
    if (!is_object())
    {
        *this = json_object();
    }
    if (!copy_and_leak())
    {
        throw json_exception(json_exception::unexpected_error);
    }
    return (to_object())[key];
}

//! Retrieve a reference of value by a key
inline const json_value &json_value::get_value(const char *key) const
{
    return (is_object() ? (to_object())[key] : json_value::invalid());
}

//! Treat self value as array by force, retrieving value at index n
inline json_value &json_value::get_value(size_type n)
{
    if (!is_array())
    {
        throw json_exception(json_exception::unmatched_type);
    }
    if (!copy_and_leak())
    {
        throw json_exception(json_exception::unexpected_error);
    }
    return (to_array())[n];
}

//! Retrieve a reference of value at index n
inline const json_value &json_value::get_value(size_type n) const
{
    return (is_array() ? (to_array())[n] : json_value::invalid());
}

//! Set the new array to the value, replacing its current content
inline bool json_value::set_value(const json_array &val)
{
    if (copy_on_write())
    {
        mod_json_value_assign_array(val_, *(mod_json_array_t**)&val);
        return true;
    }
    return false;
}

//! Set the new object to the value, replacing its current content
inline bool json_value::set_value(const json_object &val)
{
    if (copy_on_write())
    {
        mod_json_value_assign_object(val_, *(mod_json_object_t**)&val);
        return true;
    }
    return false;
}

/*! JSON Parser
 */
class json_parser
{
public:
    //! Constructor
    json_parser(void) : 
        state_(mod_json_state_null), 
        error_(mod_json_error_null), 
        context_(0)
    {
        option_.options = 0;
        option_.object_depth = 0;
        option_.array_depth = 0;
    }

    //! Destructor
    ~json_parser(void)
    {
    }

    //! Set the max object depth 
    void set_object_depth(size_type depth)
    {
        option_.object_depth = depth;
    }

    //! Set the max array depth 
    void set_array_depth(size_type depth)
    {
        option_.array_depth = depth;
    }

    //! Enable/Disable comments 
    void set_comment(bool enable = true)
    {
        if (enable)
        {
            option_.options |= MOD_JSON_COMMENT;
        }
        else
        {
            option_.options &= ~MOD_JSON_COMMENT;
        }
    }

    //! Enable/Disable loose strings 
    void set_unstrict(bool enable = true)
    {
        if (enable)
        {
            option_.options |= MOD_JSON_UNSTRICT;
        }
        else
        {
            option_.options &= ~MOD_JSON_UNSTRICT;
        }
    }

    //! Enable/Disable simple format
    void set_simple(bool enable = true)
    {
        if (enable)
        {
            option_.options |= MOD_JSON_SIMPLE;
        }
        else
        {
            option_.options &= ~MOD_JSON_SIMPLE;
        }
    }

    //! Enable/Disable single quotes support
    void set_squote(bool enable = true)
    {
        if (enable)
        {
            option_.options |= MOD_JSON_SQUOTE;
        }
        else
        {
            option_.options &= ~MOD_JSON_SQUOTE;
        }
    }

    //! Convert a sting to a JSON value 
    bool parse(json_value &val, const char *str)
    {
        mod_json_token_t *tok;

        state_ = mod_json_state_null;
        error_ = mod_json_error_null;
        context_ = str;

        tok = mod_json_token_create(&option_);
        if (tok)
        {
            mod_json_value_t *jval;

            jval = mod_json_parse(tok, str);

            /* save information of token */
            state_ = mod_json_token_state(tok); 
            error_ = mod_json_token_error(tok); 
            context_ = mod_json_token_context(tok); 
            mod_json_token_destroy(tok);

            if (jval)
            {
                val = *reinterpret_cast<json_value*>(&jval);
                mod_json_value_unset(jval);

                return val.is_valid();
            }
        }
        return false;
    }

    //! Retrieve the error code of parser 
    int error(void) const
    {
        return (int)error_;
    }

    //! Retrieve the state code of parser 
    int state(void) const
    {
        return (int)state_;
    }

    //! Retrieve the context of parser 
    const char *context(void) const
    {
        return context_;
    }

private:
    mod_json_option_t  option_;
    mod_json_state_t   state_;
    mod_json_error_t   error_;
    mod_json_cchar_t * context_;

};

/*! JSON Dumper
 */
class json_dumper
{
public:
    //! Constructor
    json_dumper(void) : str_()
    {
    }

    //! Destructor
    ~json_dumper(void)
    {
    }

    //! Dump a JSON value to string 
    bool dump(const json_value &val)
    {
        mod_json_string_t *str;

        str = mod_json_dump(*((mod_json_value_t**)&val));
        str_ = *reinterpret_cast<json_string*>(&str);
        if (str)
        {
            mod_json_string_unset(str);
            return true;
        }
        return false;
    }

    //! Retrieve result of dumper 
    json_string &result(void)
    {
        return str_;
    }

    //! Retrieve result of dumper
    const json_string &result(void) const
    {
        return str_;
    }

private:
    json_string  str_;

};

MODFRAME_NS_END

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_string &lhs, const char *rhs)
{
    const char *self = lhs.c_str();
    if (self == rhs)
    {
        return true;
    }

    if (self && rhs)
    {
        return (std::strcmp(self, rhs) == 0);
    }
    return false;
}

//! Equality
static inline
bool operator==(const char *lhs, const MODFRAME_NS::json_string &rhs)
{
    return (rhs == lhs);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_string &lhs, const std::string &rhs)
{
    std::size_t ls = lhs.size();
    std::size_t rs = rhs.size();
    if (ls == 0 && rs == 0)
    {
        return true;
    }

    if (ls == rs)
    {
        const char *ld = lhs.data();
        const char *rd = rhs.data();

        if (ld && rd)
        {
            return (std::memcmp(ld, rd, ls) == 0);
        }
    }
    return false;
}

//! Equality
static inline
bool operator==(const std::string &lhs, const MODFRAME_NS::json_string &rhs)
{
    return (rhs == lhs);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_string &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_string() ? lhs == rhs.as_string() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, 
                const MODFRAME_NS::json_string &rhs)
{
    return (lhs.is_string() ? lhs.as_string() == rhs : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_array &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_array() ? lhs == rhs.as_array() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, 
                const MODFRAME_NS::json_array &rhs)
{
    return (lhs.is_array() ? lhs.as_array() == rhs : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_object &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_object() ? lhs == rhs.as_object() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, 
                const MODFRAME_NS::json_object &rhs)
{
    return (lhs.is_object() ? lhs.as_object() == rhs : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const bool &rhs)
{
    return (lhs.is_boolean() ? lhs.as_bool() == rhs : false);
}

//! Equality
static inline
bool operator==(const bool &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_boolean() ? lhs == rhs.as_bool() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const char &rhs)
{
    return (lhs.is_integer() ? lhs.as_llong() == rhs : false);
}

//! Equality
static inline
bool operator==(const char &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_llong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const short &rhs)
{
    return (lhs.is_integer() ? lhs.as_llong() == rhs : false);
}

//! Equality
static inline
bool operator==(const short &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_llong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const int &rhs)
{
    return (lhs.is_integer() ? lhs.as_llong() == rhs : false);
}

//! Equality
static inline
bool operator==(const int &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_llong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const long &rhs)
{
    return (lhs.is_integer() ? lhs.as_llong() == rhs : false);
}

//! Equality
static inline
bool operator==(const long &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_llong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const long long &rhs)
{
    return (lhs.is_integer() ? lhs.as_llong() == rhs : false);
}

//! Equality
static inline
bool operator==(const long long &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_llong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const float &rhs)
{
    if (lhs.is_float())
    {
        double diff = lhs.as_double() - rhs;
        return ((diff < DBL_EPSILON) && (diff > -DBL_EPSILON));
    }
    return false;
}

//! Equality
static inline
bool operator==(const float &lhs, const MODFRAME_NS::json_value &rhs)
{
    if (rhs.is_float())
    {
        double diff = rhs.as_double() - lhs;
        return ((diff < DBL_EPSILON) && (diff > -DBL_EPSILON));
    }
    return false;
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const double &rhs)
{
    if (lhs.is_float())
    {
        double diff = lhs.as_double() - rhs;
        return ((diff < DBL_EPSILON) && (diff > -DBL_EPSILON));
    }
    return false;
}

//! Equality
static inline
bool operator==(const double &lhs, const MODFRAME_NS::json_value &rhs)
{
    if (rhs.is_float())
    {
        double diff = rhs.as_double() - lhs;
        return ((diff < DBL_EPSILON) && (diff > -DBL_EPSILON));
    }
    return false;
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const unsigned char &rhs)
{
    return (lhs.is_integer() ? lhs.as_ullong() == rhs : false);
}

//! Equality
static inline
bool operator==(const unsigned char &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_ullong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, 
                const unsigned short &rhs)
{
    return (lhs.is_integer() ? lhs.as_ullong() == rhs : false);
}

//! Equality
static inline
bool operator==(const unsigned short &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_ullong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const unsigned int &rhs)
{
    return (lhs.is_integer() ? lhs.as_ullong() == rhs : false);
}

//! Equality
static inline
bool operator==(const unsigned int &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_ullong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const unsigned long &rhs)
{
    return (lhs.is_integer() ? lhs.as_ullong() == rhs : false);
}

//! Equality
static inline
bool operator==(const unsigned long &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_ullong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, 
                const unsigned long long &rhs)
{
    return (lhs.is_integer() ? lhs.as_ullong() == rhs : false);
}

//! Equality
static inline
bool operator==(const unsigned long long &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_integer() ? lhs == rhs.as_ullong() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const char *rhs)
{
    return (lhs.is_string() ? lhs.as_string() == rhs : false);
}

//! Equality
static inline
bool operator==(const char *lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_string() ? lhs == rhs.as_string() : false);
}

//! Equality
static inline
bool operator==(const MODFRAME_NS::json_value &lhs, const std::string &rhs)
{
    return (lhs.is_string() ? lhs.as_string() == rhs : false);
}

//! Equality
static inline
bool operator==(const std::string &lhs, const MODFRAME_NS::json_value &rhs)
{
    return (rhs.is_string() ? lhs == rhs.as_string() : false);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_string &lhs, const char *rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const char *lhs, const MODFRAME_NS::json_string &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_string &lhs, const std::string &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const std::string &lhs, const MODFRAME_NS::json_string &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_string &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, 
                const MODFRAME_NS::json_string &rhs)
{
    return !(lhs == rhs);;
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_array &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, 
                const MODFRAME_NS::json_array &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_object &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, 
                const MODFRAME_NS::json_object &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const bool &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const bool &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const char &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const char &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const short &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const short &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const int &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const int &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const long &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const long &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const long long &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const long long &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const float &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const float &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const double &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const double &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const unsigned char &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const unsigned char &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, 
                const unsigned short &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const unsigned short &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const unsigned int &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const unsigned int &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const unsigned long &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const unsigned long &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, 
                const unsigned long long &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const unsigned long long &lhs, 
                const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const char *rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const char *lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const MODFRAME_NS::json_value &lhs, const std::string &rhs)
{
    return !(lhs == rhs);
}

//! No equality
static inline
bool operator!=(const std::string &lhs, const MODFRAME_NS::json_value &rhs)
{
    return !(lhs == rhs);
}

#endif //__MOD_JSON_HH__
