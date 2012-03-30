/**\file*********************************************************************
*                                                                     \brief
*  XMLLite wrapper
*
****************************************************************************
*/
#ifndef NTL__WIN_XML
#define NTL__WIN_XML
#pragma once

#include "com.hxx"

namespace ntl {

  /// Win32 Application support
  namespace win {

    /**\addtogroup  winapi_types_support *** Win32 API support library **********
    *@{*/

    /** XML support */
    namespace xml
    {
      extern "C" const iid IID_IXmlReader;
      extern "C" const iid IID_IXmlResolver;
      extern "C" const iid IID_IXmlWriter;
      extern "C" const iid LIBID_XmlLite;

      #ifndef NTL_NO_AUTOLINK
      # pragma comment(lib, "XmlLite.lib")
      #endif

      struct IXmlReader;
      struct IXmlWriter;
      struct IXmlReaderInput;
      struct IXmlWriterOutput;

      using com::IUnknown;
      using com::IMalloc;

      NTL_EXTERNAPI hresult __stdcall CreateXmlReader(const _GUID& riid, IXmlReader** ppvObject, IMalloc* pMalloc);
      NTL_EXTERNAPI hresult __stdcall CreateXmlWriter(const iid& riid, IXmlWriter** ppvObject, IMalloc* pMalloc);

      NTL_EXTERNAPI hresult __stdcall CreateXmlReaderInputWithEncodingCodePage(IUnknown* pInputStream, IMalloc* pMalloc, uint32_t EncodingCodePage, boolean fEncodingHint, const wchar_t* pwszBaseUri, IXmlReaderInput** ppInput);
      NTL_EXTERNAPI hresult __stdcall CreateXmlReaderInputWithEncodingName(IUnknown* pInputStream, IMalloc* pMalloc, const wchar_t* EncodingCodeName, boolean fEncodingHint, const wchar_t* pwszBaseUri, IXmlReaderInput** ppInput);

      NTL_EXTERNAPI hresult __stdcall CreateXmlWriterOutputWithEncodingCodePage(IUnknown* pOutputStream, IMalloc* pMalloc, uint32_t EncodingCodePage, IXmlWriterOutput** ppObject);
      NTL_EXTERNAPI hresult __stdcall CreateXmlWriterOutputWithEncodingName(IUnknown* pOutputStream, IMalloc* pMalloc, const wchar_t* EncodingCodeName, IXmlWriterOutput** ppObject);

      enum XmlNodeType
      {
        XmlNodeType_None	= 0,
        XmlNodeType_Element	= 1,
        XmlNodeType_Attribute	= 2,
        XmlNodeType_Text	= 3,
        XmlNodeType_CDATA	= 4,
        XmlNodeType_ProcessingInstruction	= 7,
        XmlNodeType_Comment	= 8,
        XmlNodeType_DocumentType	= 10,
        XmlNodeType_Whitespace	= 13,
        XmlNodeType_EndElement	= 15,
        XmlNodeType_XmlDeclaration	= 17,
        _XmlNodeType_Last	= 17
      };

      enum XmlConformanceLevel
      {
        XmlConformanceLevel_Auto	= 0,
        XmlConformanceLevel_Fragment	= 1,
        XmlConformanceLevel_Document	= 2,
        _XmlConformanceLevel_Last	= 2
      };


      enum DtdProcessing
      {
        DtdProcessing_Prohibit	= 0,
        DtdProcessing_Parse	= ( DtdProcessing_Prohibit + 1 ) ,
        _DtdProcessing_Last	= DtdProcessing_Parse
      };

      enum XmlReadState
      {
        XmlReadState_Initial	= 0,
        XmlReadState_Interactive	= 1,
        XmlReadState_Error	= 2,
        XmlReadState_EndOfFile	= 3,
        XmlReadState_Closed	= 4
      };

      enum XmlReaderProperty
      {
        XmlReaderProperty_MultiLanguage	= 0,
        XmlReaderProperty_ConformanceLevel	= ( XmlReaderProperty_MultiLanguage + 1 ) ,
        XmlReaderProperty_RandomAccess	= ( XmlReaderProperty_ConformanceLevel + 1 ) ,
        XmlReaderProperty_XmlResolver	= ( XmlReaderProperty_RandomAccess + 1 ) ,
        XmlReaderProperty_DtdProcessing	= ( XmlReaderProperty_XmlResolver + 1 ) ,
        XmlReaderProperty_ReadState	= ( XmlReaderProperty_DtdProcessing + 1 ) ,
        XmlReaderProperty_MaxElementDepth	= ( XmlReaderProperty_ReadState + 1 ) ,
        XmlReaderProperty_MaxEntityExpansion	= ( XmlReaderProperty_MaxElementDepth + 1 ) ,
        _XmlReaderProperty_Last	= XmlReaderProperty_MaxEntityExpansion
      };

      enum XmlError
      {
        MX_E_MX	= 0xc00cee00,
        MX_E_INPUTEND	= ( MX_E_MX + 1 ) ,
        MX_E_ENCODING	= ( MX_E_INPUTEND + 1 ) ,
        MX_E_ENCODINGSWITCH	= ( MX_E_ENCODING + 1 ) ,
        MX_E_ENCODINGSIGNATURE	= ( MX_E_ENCODINGSWITCH + 1 ) ,
        WC_E_WC	= 0xc00cee20,
        WC_E_WHITESPACE	= ( WC_E_WC + 1 ) ,
        WC_E_SEMICOLON	= ( WC_E_WHITESPACE + 1 ) ,
        WC_E_GREATERTHAN	= ( WC_E_SEMICOLON + 1 ) ,
        WC_E_QUOTE	= ( WC_E_GREATERTHAN + 1 ) ,
        WC_E_EQUAL	= ( WC_E_QUOTE + 1 ) ,
        WC_E_LESSTHAN	= ( WC_E_EQUAL + 1 ) ,
        WC_E_HEXDIGIT	= ( WC_E_LESSTHAN + 1 ) ,
        WC_E_DIGIT	= ( WC_E_HEXDIGIT + 1 ) ,
        WC_E_LEFTBRACKET	= ( WC_E_DIGIT + 1 ) ,
        WC_E_LEFTPAREN	= ( WC_E_LEFTBRACKET + 1 ) ,
        WC_E_XMLCHARACTER	= ( WC_E_LEFTPAREN + 1 ) ,
        WC_E_NAMECHARACTER	= ( WC_E_XMLCHARACTER + 1 ) ,
        WC_E_SYNTAX	= ( WC_E_NAMECHARACTER + 1 ) ,
        WC_E_CDSECT	= ( WC_E_SYNTAX + 1 ) ,
        WC_E_COMMENT	= ( WC_E_CDSECT + 1 ) ,
        WC_E_CONDSECT	= ( WC_E_COMMENT + 1 ) ,
        WC_E_DECLATTLIST	= ( WC_E_CONDSECT + 1 ) ,
        WC_E_DECLDOCTYPE	= ( WC_E_DECLATTLIST + 1 ) ,
        WC_E_DECLELEMENT	= ( WC_E_DECLDOCTYPE + 1 ) ,
        WC_E_DECLENTITY	= ( WC_E_DECLELEMENT + 1 ) ,
        WC_E_DECLNOTATION	= ( WC_E_DECLENTITY + 1 ) ,
        WC_E_NDATA	= ( WC_E_DECLNOTATION + 1 ) ,
        WC_E_PUBLIC	= ( WC_E_NDATA + 1 ) ,
        WC_E_SYSTEM	= ( WC_E_PUBLIC + 1 ) ,
        WC_E_NAME	= ( WC_E_SYSTEM + 1 ) ,
        WC_E_ROOTELEMENT	= ( WC_E_NAME + 1 ) ,
        WC_E_ELEMENTMATCH	= ( WC_E_ROOTELEMENT + 1 ) ,
        WC_E_UNIQUEATTRIBUTE	= ( WC_E_ELEMENTMATCH + 1 ) ,
        WC_E_TEXTXMLDECL	= ( WC_E_UNIQUEATTRIBUTE + 1 ) ,
        WC_E_LEADINGXML	= ( WC_E_TEXTXMLDECL + 1 ) ,
        WC_E_TEXTDECL	= ( WC_E_LEADINGXML + 1 ) ,
        WC_E_XMLDECL	= ( WC_E_TEXTDECL + 1 ) ,
        WC_E_ENCNAME	= ( WC_E_XMLDECL + 1 ) ,
        WC_E_PUBLICID	= ( WC_E_ENCNAME + 1 ) ,
        WC_E_PESINTERNALSUBSET	= ( WC_E_PUBLICID + 1 ) ,
        WC_E_PESBETWEENDECLS	= ( WC_E_PESINTERNALSUBSET + 1 ) ,
        WC_E_NORECURSION	= ( WC_E_PESBETWEENDECLS + 1 ) ,
        WC_E_ENTITYCONTENT	= ( WC_E_NORECURSION + 1 ) ,
        WC_E_UNDECLAREDENTITY	= ( WC_E_ENTITYCONTENT + 1 ) ,
        WC_E_PARSEDENTITY	= ( WC_E_UNDECLAREDENTITY + 1 ) ,
        WC_E_NOEXTERNALENTITYREF	= ( WC_E_PARSEDENTITY + 1 ) ,
        WC_E_PI	= ( WC_E_NOEXTERNALENTITYREF + 1 ) ,
        WC_E_SYSTEMID	= ( WC_E_PI + 1 ) ,
        WC_E_QUESTIONMARK	= ( WC_E_SYSTEMID + 1 ) ,
        WC_E_CDSECTEND	= ( WC_E_QUESTIONMARK + 1 ) ,
        WC_E_MOREDATA	= ( WC_E_CDSECTEND + 1 ) ,
        WC_E_DTDPROHIBITED	= ( WC_E_MOREDATA + 1 ) ,
        WC_E_INVALIDXMLSPACE	= ( WC_E_DTDPROHIBITED + 1 ) ,
        NC_E_NC	= 0xc00cee60,
        NC_E_QNAMECHARACTER	= ( NC_E_NC + 1 ) ,
        NC_E_QNAMECOLON	= ( NC_E_QNAMECHARACTER + 1 ) ,
        NC_E_NAMECOLON	= ( NC_E_QNAMECOLON + 1 ) ,
        NC_E_DECLAREDPREFIX	= ( NC_E_NAMECOLON + 1 ) ,
        NC_E_UNDECLAREDPREFIX	= ( NC_E_DECLAREDPREFIX + 1 ) ,
        NC_E_EMPTYURI	= ( NC_E_UNDECLAREDPREFIX + 1 ) ,
        NC_E_XMLPREFIXRESERVED	= ( NC_E_EMPTYURI + 1 ) ,
        NC_E_XMLNSPREFIXRESERVED	= ( NC_E_XMLPREFIXRESERVED + 1 ) ,
        NC_E_XMLURIRESERVED	= ( NC_E_XMLNSPREFIXRESERVED + 1 ) ,
        NC_E_XMLNSURIRESERVED	= ( NC_E_XMLURIRESERVED + 1 ) ,
        SC_E_SC	= 0xc00cee80,
        SC_E_MAXELEMENTDEPTH	= ( SC_E_SC + 1 ) ,
        SC_E_MAXENTITYEXPANSION	= ( SC_E_MAXELEMENTDEPTH + 1 ) ,
        WR_E_WR	= 0xc00cef00,
        WR_E_NONWHITESPACE	= ( WR_E_WR + 1 ) ,
        WR_E_NSPREFIXDECLARED	= ( WR_E_NONWHITESPACE + 1 ) ,
        WR_E_NSPREFIXWITHEMPTYNSURI	= ( WR_E_NSPREFIXDECLARED + 1 ) ,
        WR_E_DUPLICATEATTRIBUTE	= ( WR_E_NSPREFIXWITHEMPTYNSURI + 1 ) ,
        WR_E_XMLNSPREFIXDECLARATION	= ( WR_E_DUPLICATEATTRIBUTE + 1 ) ,
        WR_E_XMLPREFIXDECLARATION	= ( WR_E_XMLNSPREFIXDECLARATION + 1 ) ,
        WR_E_XMLURIDECLARATION	= ( WR_E_XMLPREFIXDECLARATION + 1 ) ,
        WR_E_XMLNSURIDECLARATION	= ( WR_E_XMLURIDECLARATION + 1 ) ,
        WR_E_NAMESPACEUNDECLARED	= ( WR_E_XMLNSURIDECLARATION + 1 ) ,
        WR_E_INVALIDXMLSPACE	= ( WR_E_NAMESPACEUNDECLARED + 1 ) ,
        WR_E_INVALIDACTION	= ( WR_E_INVALIDXMLSPACE + 1 ) ,
        WR_E_INVALIDSURROGATEPAIR	= ( WR_E_INVALIDACTION + 1 ) ,
        XML_E_INVALID_DECIMAL	= 0xc00ce01d,
        XML_E_INVALID_HEXIDECIMAL	= 0xc00ce01e,
        XML_E_INVALID_UNICODE	= 0xc00ce01f,
        XML_E_INVALIDENCODING	= 0xc00ce06e
      };

      enum XmlStandalone
      {
        XmlStandalone_Omit	= 0,
        XmlStandalone_Yes	= 1,
        XmlStandalone_No	= 2,
        _XmlStandalone_Last	= 2
      };

      enum XmlWriterProperty
      {
        XmlWriterProperty_MultiLanguage	= 0,
        XmlWriterProperty_Indent	= ( XmlWriterProperty_MultiLanguage + 1 ) ,
        XmlWriterProperty_ByteOrderMark	= ( XmlWriterProperty_Indent + 1 ) ,
        XmlWriterProperty_OmitXmlDeclaration	= ( XmlWriterProperty_ByteOrderMark + 1 ) ,
        XmlWriterProperty_ConformanceLevel	= ( XmlWriterProperty_OmitXmlDeclaration + 1 ) ,
        _XmlWriterProperty_Last	= XmlWriterProperty_OmitXmlDeclaration
      };

    struct __declspec(uuid("7279FC81-709D-4095-B63D-69FE4B0D9030")) __declspec(novtable)
    IXmlReader : public IUnknown
    {
    public:
      virtual hresult __stdcall SetInput( 
        /*  */ 
        __in_opt  IUnknown *pInput) = 0;

      virtual hresult __stdcall GetProperty( 
        /*  */ 
        __in  XmlReaderProperty nProperty,
        /*  */ 
        __out  long* *ppValue) = 0;

      virtual hresult __stdcall SetProperty( 
        /*  */ 
        __in  XmlReaderProperty nProperty,
        /*  */ 
        __in_opt  long* pValue) = 0;

      virtual hresult __stdcall Read( 
        /*  */ 
        __out_opt  XmlNodeType *pNodeType) = 0;

      virtual hresult __stdcall GetNodeType( 
        /*  */ 
        __out  XmlNodeType *pNodeType) = 0;

      virtual hresult __stdcall MoveToFirstAttribute(void) = 0;

      virtual hresult __stdcall MoveToNextAttribute(void) = 0;

      virtual hresult __stdcall MoveToAttributeByName( 
        /*  */ 
        __in  const wchar_t* pwszLocalName,
        /*  */ 
        __in_opt  const wchar_t* pwszNamespaceUri) = 0;

      virtual hresult __stdcall MoveToElement(void) = 0;

      virtual hresult __stdcall GetQualifiedName( 
        /*  */ 
        __out  const wchar_t* *ppwszQualifiedName,
        /*  */ 
        __out_opt  uint32_t *pcwchQualifiedName) = 0;

      virtual hresult __stdcall GetNamespaceUri( 
        /*  */ 
        __out  const wchar_t* *ppwszNamespaceUri,
        /*  */ 
        __out_opt  uint32_t *pcwchNamespaceUri) = 0;

      virtual hresult __stdcall GetLocalName( 
        /*  */ 
        __out  const wchar_t* *ppwszLocalName,
        /*  */ 
        __out_opt  uint32_t *pcwchLocalName) = 0;

      virtual hresult __stdcall GetPrefix( 
        /*  */ 
        __out  const wchar_t* *ppwszPrefix,
        /*  */ 
        __out_opt  uint32_t *pcwchPrefix) = 0;

      virtual hresult __stdcall GetValue( 
        /*  */ 
        __out  const wchar_t* *ppwszValue,
        /*  */ 
        __out_opt  uint32_t *pcwchValue) = 0;

      virtual hresult __stdcall ReadValueChunk( 
        /*  */ 
        wchar_t *pwchBuffer,
        /*  */ 
        __in  uint32_t cwchChunkSize,
        /*  */ 
        __inout  uint32_t *pcwchRead) = 0;

      virtual hresult __stdcall GetBaseUri( 
        /*  */ 
        __out  const wchar_t* *ppwszBaseUri,
        /*  */ 
        __out_opt  uint32_t *pcwchBaseUri) = 0;

      virtual boolean __stdcall IsDefault(void) = 0;

      virtual boolean __stdcall IsEmptyElement(void) = 0;

      virtual hresult __stdcall GetLineNumber( 
        /*  */ 
        __out  uint32_t *pnLineNumber) = 0;

      virtual hresult __stdcall GetLinePosition( 
        /*  */ 
        __out  uint32_t *pnLinePosition) = 0;

      virtual hresult __stdcall GetAttributeCount( 
        /*  */ 
        __out  uint32_t *pnAttributeCount) = 0;

      virtual hresult __stdcall GetDepth( 
        /*  */ 
        __out  uint32_t *pnDepth) = 0;

      virtual boolean __stdcall IsEOF(void) = 0;

    };

    struct __declspec(novtable) __declspec(uuid("7279FC82-709D-4095-B63D-69FE4B0D9030"))
    IXmlResolver : public IUnknown
    {
    public:
      virtual hresult __stdcall ResolveUri( 
        /*  */ 
        __in_opt  const wchar_t* pwszBaseUri,
        /*  */ 
        __in_opt  const wchar_t* pwszPublicIdentifier,
        /*  */ 
        __in_opt  const wchar_t* pwszSystemIdentifier,
        /*  */ 
        __out  IUnknown **ppResolvedInput) = 0;

    };

    struct __declspec(novtable) __declspec(uuid("7279FC88-709D-4095-B63D-69FE4B0D9030"))
    IXmlWriter : public IUnknown
    {
    public:
      virtual hresult __stdcall SetOutput( 
        /*  */ 
        __in_opt  IUnknown *pOutput) = 0;

      virtual hresult __stdcall GetProperty( 
        /*  */ 
        __in  XmlWriterProperty nProperty,
        /*  */ 
        __out  long* *ppValue) = 0;

      virtual hresult __stdcall SetProperty( 
        /*  */ 
        __in  XmlWriterProperty nProperty,
        /*  */ 
        __in_opt  long* pValue) = 0;

      virtual hresult __stdcall WriteAttributes( 
        /*  */ 
        __in  IXmlReader *pReader,
        /*  */ 
        __in  boolean fWriteDefaultAttributes) = 0;

      virtual hresult __stdcall WriteAttributeString( 
        /*  */ 
        __in_opt  const wchar_t* pwszPrefix,
        /*  */ 
        __in_opt  const wchar_t* pwszLocalName,
        /*  */ 
        __in_opt  const wchar_t* pwszNamespaceUri,
        /*  */ 
        __in_opt  const wchar_t* pwszValue) = 0;

      virtual hresult __stdcall WriteCData( 
        /*  */ 
        __in_opt  const wchar_t* pwszText) = 0;

      virtual hresult __stdcall WriteCharEntity( 
        /*  */ 
        __in  wchar_t wch) = 0;

      virtual hresult __stdcall WriteChars( 
        /*  */ 
        const wchar_t *pwch,
        /*  */ 
        __in  uint32_t cwch) = 0;

      virtual hresult __stdcall WriteComment( 
        /*  */ 
        __in_opt  const wchar_t* pwszComment) = 0;

      virtual hresult __stdcall WriteDocType( 
        /*  */ 
        __in_opt  const wchar_t* pwszName,
        /*  */ 
        __in_opt  const wchar_t* pwszPublicId,
        /*  */ 
        __in_opt  const wchar_t* pwszSystemId,
        /*  */ 
        __in_opt  const wchar_t* pwszSubset) = 0;

      virtual hresult __stdcall WriteElementString( 
        /*  */ 
        __in_opt  const wchar_t* pwszPrefix,
        /*  */ 
        __in  const wchar_t* pwszLocalName,
        /*  */ 
        __in_opt  const wchar_t* pwszNamespaceUri,
        /*  */ 
        __in_opt  const wchar_t* pwszValue) = 0;

      virtual hresult __stdcall WriteEndDocument(void) = 0;

      virtual hresult __stdcall WriteEndElement(void) = 0;

      virtual hresult __stdcall WriteEntityRef( 
        /*  */ 
        __in  const wchar_t* pwszName) = 0;

      virtual hresult __stdcall WriteFullEndElement(void) = 0;

      virtual hresult __stdcall WriteName( 
        /*  */ 
        __in  const wchar_t* pwszName) = 0;

      virtual hresult __stdcall WriteNmToken( 
        /*  */ 
        __in  const wchar_t* pwszNmToken) = 0;

      virtual hresult __stdcall WriteNode( 
        /*  */ 
        __in  IXmlReader *pReader,
        /*  */ 
        __in  boolean fWriteDefaultAttributes) = 0;

      virtual hresult __stdcall WriteNodeShallow( 
        /*  */ 
        __in  IXmlReader *pReader,
        /*  */ 
        __in  boolean fWriteDefaultAttributes) = 0;

      virtual hresult __stdcall WriteProcessingInstruction( 
        /*  */ 
        __in  const wchar_t* pwszName,
        /*  */ 
        __in_opt  const wchar_t* pwszText) = 0;

      virtual hresult __stdcall WriteQualifiedName( 
        /*  */ 
        __in  const wchar_t* pwszLocalName,
        /*  */ 
        __in_opt  const wchar_t* pwszNamespaceUri) = 0;

      virtual hresult __stdcall WriteRaw( 
        /*  */ 
        __in_opt  const wchar_t* pwszData) = 0;

      virtual hresult __stdcall WriteRawChars( 
        /*  */ 
        const wchar_t *pwch,
        /*  */ 
        __in  uint32_t cwch) = 0;

      virtual hresult __stdcall WriteStartDocument( 
        /*  */ 
        __in  XmlStandalone standalone) = 0;

      virtual hresult __stdcall WriteStartElement( 
        /*  */ 
        __in_opt  const wchar_t* pwszPrefix,
        /*  */ 
        __in  const wchar_t* pwszLocalName,
        /*  */ 
        __in_opt  const wchar_t* pwszNamespaceUri) = 0;

      virtual hresult __stdcall WriteString( 
        /*  */ 
        __in_opt  const wchar_t* pwszText) = 0;

      virtual hresult __stdcall WriteSurrogateCharEntity( 
        /*  */ 
        __in  wchar_t wchLow,
        /*  */ 
        __in  wchar_t wchHigh) = 0;

      virtual hresult __stdcall WriteWhitespace( 
        /*  */ 
        __in_opt  const wchar_t* pwszWhitespace) = 0;

      virtual hresult __stdcall Flush(void) = 0;

    };

  }

  /**@} winapi_types_support */


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_XML
