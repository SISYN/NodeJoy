/* Copyright (c) 2018 Dan Lindsey. See the file LICENSE for copying permission. */

var C = {
  MY : 0x001,          // description
  PRIVATE : 0x001,     // description
  CONSTANTS : 0x00423  // description
};

function Cryptography() {

}

/** 'public' constants here */
Cryptography.prototype.C = {
  MY : 0x013,         // description
  PUBLIC : 0x0541,    // description
  CONSTANTS : 0x023   // description
};


/** Put most of my comments outside the functions... */
Cryptography.prototype.foobar = function() {

};

/**
 * Encrypts text using Corrected Block TEA (xxtea) algorithm.
 *
 * @param   {string} plaintext - String to be encrypted (multi-byte safe).
 * @param   {string} password - Password to be used for encryption (1st 16 chars).
 * @returns {string} Encrypted text (encoded as base64).
 */
exports.encrypt = function(plaintext, password) {
    plaintext = String(plaintext);
    password = String(password);
    if (plaintext.length === 0) return('');  // nothing to encrypt
    //  v is n-word data vector; converted to array of longs from UTF-8 string
    var v = this.strToLongs(plaintext);
    //  k is 4-word key; simply convert first 16 chars of password as key
    var k = this.strToLongs(password.slice(0,16));
    var n = v.length;
    v = this.encode(v, k);
    // convert array of longs to string
    var ciphertext = this.longsToStr(v);
    // convert binary string to base64 ascii for safe transport
    return btoa(ciphertext);
};


/**
 * Decrypts text using Corrected Block TEA (xxtea) algorithm.
 *
 * @param   {string} ciphertext - String to be decrypted.
 * @param   {string} password - Password to be used for decryption (1st 16 chars).
 * @returns {string} Decrypted text.
 */
exports.decrypt = function(ciphertext, password) {
    ciphertext = String(ciphertext);
    password = String(password);
    if (ciphertext.length === 0) return('');
    //  v is n-word data vector; converted to array of longs from base64 string
    var v = this.strToLongs(atob(ciphertext));
    //  k is 4-word key; simply convert first 16 chars of password as key
    var k = this.strToLongs(password.slice(0,16));
    var n = v.length;
    v = this.decode(v, k);
    var plaintext = this.longsToStr(v);
    // strip trailing null chars resulting from filling 4-char blocks:
    var lastCh = plaintext.length-1;
    while (plaintext[lastCh]===0) lastCh--;
    plaintext = plaintext.substr(0,lastCh+1);
    return plaintext;
};

/**
 * XXTEA: encodes array of unsigned 32-bit integers using 128-bit key.
 *
 * @param   {number[]} v - Data vector.
 * @param   {number[]} k - Key.
 * @returns {number[]} Encoded vector.
 */
exports.encode = function(v, k) {
    if (v.length < 2) v[1] = 0;  // algorithm doesn't work for n<2 so fudge by adding a null
    var n = v.length;
    var z = v[n-1], y = v[0], delta = 0x9E3779B9;
    var mx, e, q = Math.floor(6 + 52/n), sum = 0;
    while (q-- > 0) {  // 6 + 52/n operations gives between 6 & 32 mixes on each word
        sum += delta;
        e = sum>>>2 & 3;
        for (var p = 0; p < n; p++) {
            y = v[(p+1)%n];
            mx = (z>>>5 ^ y<<2) + (y>>>3 ^ z<<4) ^ (sum^y) + (k[p&3 ^ e] ^ z);
            z = v[p] += mx;
        }
    }
    return v;
};
/**
 * XXTEA: decodes array of unsigned 32-bit integers using 128-bit key.
 *
 * @param   {number[]} v - Data vector.
 * @param   {number[]} k - Key.
 * @returns {number[]} Decoded vector.
 */
exports.decode = function(v, k) {
    var n = v.length;
    var z = v[n-1], y = v[0], delta = 0x9E3779B9;
    var mx, e, q = Math.floor(6 + 52/n), sum = q*delta;
    while (sum !== 0) {
        e = sum>>>2 & 3;
        for (var p = n-1; p >= 0; p--) {
            z = v[p>0 ? p-1 : n-1];
            mx = (z>>>5 ^ y<<2) + (y>>>3 ^ z<<4) ^ (sum^y) + (k[p&3 ^ e] ^ z);
            y = v[p] -= mx;
        }
        sum -= delta;
    }
    return v;
};
/**
 * Converts string to array of longs (each containing 4 chars).
 * @private
 */
exports.strToLongs = function(s) {
    // note chars must be within ISO-8859-1 (Unicode code-point <= U+00FF) to fit 4/long
    var l = new Array(0|Math.ceil(s.length/4));
    for (var i=0; i<l.length; i++) {
        // note little-endian encoding - endianness is irrelevant as long as it matches longsToStr()
        l[i] = s.charCodeAt(i*4)        + (s.charCodeAt(i*4+1)<<8) +
              (s.charCodeAt(i*4+2)<<16) + (s.charCodeAt(i*4+3)<<24);
    }
    return l; // note running off the end of the string generates nulls since bitwise operators
};            // treat NaN as 0
/**
 * Converts array of longs to string.
 * @private
 */
exports.longsToStr = function(l) {
    var a = new Array(l.length);
    for (var i=0; i<l.length; i++) {
        a[i] = String.fromCharCode(l[i] & 0xFF, l[i]>>>8 & 0xFF, l[i]>>>16 & 0xFF, l[i]>>>24 & 0xFF);
    }
    return a.join('');  // use Array.join() for better performance than repeated string appends
};
