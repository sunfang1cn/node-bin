/**
 * node binary buffer helper
 * lib/binding.js
 **/

var Main = require('../Build/Release/main').Main;
var main = new Main();
var convert = require('iconv-lite');

var nodeNativeCharset = {'utf8': true, 'utf-8': true, 'ucs2': true, 'ascii': true, 'binary': true, 'base64': true};

Buffer.prototype.position = 0;

Buffer.prototype.getPosition = function () {
  return this.position || 0;
};

Buffer.prototype.isBigEndian = main.isBigEndian();

Buffer.prototype.setBigEndian = function () {
  this.isBigEndian = true;
};

Buffer.prototype.setSmallEndian = function () {
  this.isBigEndian = false;
};

Buffer.prototype.getChar = function () {
  this.position += 1;
  if (this.position >= this.length) {
    return null;
  }
  return this.readUInt8(this.position - 1).toString();
};

Buffer.prototype.getShort = function () {
  if (this.position + 2 >= this.length) {
    return null;
  }
  if (this.isBigEndian) {
    var ret = this.readInt16BE(this.position);
  } else {
    var ret = this.readInt16LE(this.position);
  }
  this.position += 2;
  return ret;
};

Buffer.prototype.getInt = function () {
  if (this.position + 4 >= this.length) {
    return null;
  }
  if (this.isBigEndian) {
    var ret = this.readInt32BE(this.position);
  } else {
    var ret = this.readInt32LE(this.position);
  }
  this.position += 4;
  return ret;
};

Buffer.prototype.getFloat = function () {
  this.position += 4;
  if (this.position >= this.length) {
    return null;
  }
  // we only use ieee754 formal type
  return this.readFloatLE(this.position - 1);
};

Buffer.prototype.getDouble = function () {
  this.position += 8;
  // we only use ieee754 formal type
  return this.readDoubleLE(this.position - 1);
};

Buffer.prototype.getString = function (offset, charset) {
  charset = (charset || 'utf-8').toLowerCase();
  offset = offset | 0;
  if (offset === 0 || offset + this.position >= this.length) {
    return null;
  }
  var _buf = new Buffer(offset + 1);
  this.copy(_buf, 0, this.position, this.position + offset);
  _buf[_buf.length - 1] = 0;
  this.position += offset;
  if (nodeNativeCharset[charset]) {
    //use native decoder
    return _buf.toString(charset);
  } else {
    // use iconv-lite
    return convert.decode(_buf, charset);
  }
};