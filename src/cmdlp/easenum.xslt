<?xml version="1.0" encoding="UTF-8"?>
<!-- This file is part of the command-line option library, which was cloned from: -->
<!-- https://github.com/masaers/cmdlp (v0.4.1 tag) -->

<!-- Thanks Markus! -->
<!-- Consider cloning the original repository if you like it. -->

<!-- Copyright (c) 2018 Markus S. Saers -->

<!-- ======================================================================= -->
<!-- Creates a C++ header with a [S]erializable [ENUM]eration. -->
<!-- These are especially useful for categorial knobs in the command line parser. -->
<!-- ======================================================================= -->
<!-- Usage: xsltproc easenum.xslt input.xml > output.hpp -->
<!-- ======================================================================= -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" />
  <xsl:strip-space elements="*"/>

  <xsl:template match="/enums">
    <xsl:text>#pragma once
/**
 * Auto-generated header file.
 */
#include &lt;iostream&gt;
#include &lt;array&gt;
</xsl:text>
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="enum">
    <xsl:if test="@namespace">
      <xsl:text>namespace </xsl:text>
      <xsl:value-of select="@namespace" />
      <xsl:text> { </xsl:text>
    </xsl:if>
    <xsl:text>class </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text> {&#xa;typedef std::array&lt;const char*,</xsl:text>
    <xsl:value-of select="count(item)" />
    <xsl:text>&gt; container_type;&#xa;</xsl:text>
    <!-- Private section -->
    <xsl:text>private:&#xa;</xsl:text>
    <xsl:text>typedef uint8_t id_type;&#xa;</xsl:text>
    <xsl:text>static const container_type value_names, value_descs;&#xa;</xsl:text>
    <!-- Public section -->
    <xsl:text>public:&#xa;</xsl:text>
    <xsl:text>enum value : id_type {</xsl:text>
    <xsl:for-each select="item">
      <xsl:if test="position() != 1">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:value-of select="@name" />
    </xsl:for-each>
    <xsl:text>};&#xa;</xsl:text>
    <xsl:text>static inline std::string desc() {
  std::ostringstream s;
  for (id_type i = 0; i &lt; value_names.size(); ++i) {
    if (i != 0) { s &lt;&lt; " "; }
    s &lt;&lt; '[' &lt;&lt; value_names[i] &lt;&lt; "] " &lt;&lt; value_descs[i];
  }
  return s.str();
}
static inline const char* desc(const value&amp; x) { return value_descs[(id_type)x]; }
static inline const char* to_cstr(const value&amp; x) { return value_names[(id_type)x]; }
static inline value from_cstr(const char* cstr) {
  id_type result = value_names.size();
  for (id_type i = 0; i &lt; result; ++i) {
    if (strcmp(value_names[i], cstr) == 0) {
      result = i;
    }
  }
  if (result == value_names.size()) {
    std::ostringstream s;
    s &lt;&lt; "Cannot interpret '" &lt;&lt; cstr &lt;&lt; "'; excpeted one of {";
    for (id_type i = 0; i &lt; value_names.size(); ++i) {
      if (i != 0) { s &lt;&lt; ','; }
      s &lt;&lt; to_cstr((value)i);
    }
    s &lt;&lt; "}.";
    throw std::runtime_error(s.str());
  }
  return (value)result;
}
</xsl:text>
    
    <!-- End of class -->
    <xsl:text>};&#xa;</xsl:text>
    <!-- Array of enum names -->
    <xsl:text>const </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::container_type </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::value_names = {</xsl:text>
    <xsl:for-each select="item">
      <xsl:if test="position() != 1">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:text>"</xsl:text>
      <xsl:value-of select="@name" />
      <xsl:text>"</xsl:text>
    </xsl:for-each>
    <xsl:text>};&#xa;</xsl:text>
    <!-- Array of enum descriptions -->
    <xsl:text>const </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::container_type </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::value_descs = {</xsl:text>
    <xsl:for-each select="item">
      <xsl:if test="position() != 1">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:text>"</xsl:text>
      <xsl:value-of select="." />
      <xsl:text>"</xsl:text>
    </xsl:for-each>
    <xsl:text>};&#xa;</xsl:text>
    <!-- (de)serializer functions -->
    <xsl:text>inline std::ostream&amp; operator&lt;&lt;(std::ostream&amp; os, const </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::value&amp; x) { return os &lt;&lt; </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::to_cstr(x); }&#xa;</xsl:text>
    <xsl:text>inline std::istream&amp; operator&gt;&gt;(std::istream&amp; is, </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::value&amp; x) { std::string str; is &gt;&gt; str; x = </xsl:text>
    <xsl:value-of select="@name" />
    <xsl:text>::from_cstr(str.c_str()); return is; }&#xa;</xsl:text>
    <!-- End namespace -->
    <xsl:if test="@namespace">
      <xsl:text>}&#xa;</xsl:text>
    </xsl:if>
  </xsl:template>

</xsl:transform>
