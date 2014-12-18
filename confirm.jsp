<!-- ******************************************************************************************************
     This example is somewhat of a mixture of HTML, JSP, and JSTL.  This was just one error
     page from the project that needed to be quite dynamic in its messages.  The app is a 
     survey, and to show the proper questions and answers, we chose to store the dynamic text
     in property files within the project, then read the text based on the language indicator
     and other parameters.  The error pages eventually went the same route, as can be seen below
     ****************************************************************************************************** -->

<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/fmt" prefix="fmt" %>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>

<!-- ******************************************************************************************************
     I use the JSTL 'fmt' library to read from the Properties file that corresponds to the chosen language.
     ****************************************************************************************************** -->
<fmt:setBundle basename="Properties_${langInd}" var="msg"/>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>

<!-- ******************************************************************************************************
     The "utf-8" probably looks a little strange.  It was the only way our system would correctly interpret
     character sets for Mandarin/Vietnamese/Cantonese.  They were very interesting languages to work with!
     ****************************************************************************************************** -->
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>The Error Page</title>
    <link rel="stylesheet" href="stylesheet_${langInd}.css" type="text/css" media="all" />
</head>
<body id="body" class="main">
<div id="wrapper">
    <div id="header">
		<jsp:include page="header_${langInd}.jsp" />
   	</div>
   	<div>

<!-- ******************************************************************************************************
     Here, you can see I am pulling in the text labeled "customerInput.text" from the Properties file
     ****************************************************************************************************** -->
    	<div id="containerTop" class="question">
    		<p><fmt:message key="customerInput.text" bundle="${msg}" /></p>
    	</div>
	</div>
   	<div id="header5"></div>
  	<div class="container">
     	<div class="container-content">

<!-- ******************************************************************************************************
     Here's where it gets interesting.  I use the JSTL core library to check the type of error sent from
     the servlet.  The error message on the page (pulled from the Properties file) is based on the error type.  
     ****************************************************************************************************** -->
     		<div class="error-container">
    			<form method="post" action="survey">
					<input type="hidden" name="p" value="customerInput">
    				<c:set var="errorType" value="${error}" />
    				<c:if test="${errorType == 'invalid'}">
						<input type="hidden" name="error" value="true">
						<p class="errorText1"><br><br><br><fmt:message key="invalid1" bundle="${msg}" /></p>
						<p class="errorText2"><fmt:message key="invalid2" bundle="${msg}" /></p>
						<p class="errorText3"><fmt:message key="invalid3" bundle="${msg}" /></p><br><br><br>
    				</c:if>
    				<c:if test="${errorType == 'notFound'}">
						<p class="errorText1"><br><br><br><br><fmt:message key="notfound1" bundle="${msg}" /></p><br>
						<p class="errorText2"><fmt:message key="notfound2" bundle="${msg}" /></p>
						<p class="errorText3"><fmt:message key="notfound3" bundle="${msg}" /></p>
    				</c:if>
    				<p class="sub_para"><br><br><fmt:message key="q.tail1" bundle="${msg}" /><br>
    				<fmt:message key="q.tail2" bundle="${msg}" /><br><br>
    				<c:if test="${errorType == 'invalid'}">
    				    <fmt:message key="qinv.tail3" bundle="${msg}" /><br><br>
    				</c:if>
    				<c:if test="${errorType == 'notFound'}">
    				    <fmt:message key="qnoans.tail3" bundle="${msg}" /><br><br>
    				</c:if>
			   		<fmt:message key="q.tail4" bundle="${msg}" /></p>
			   		<c:if test="${errorType == 'invalid'}">
			   			<input type="image" src="imgs/btnReturn_${langInd}.png" name="image" width="<fmt:message key="btnsz.return" bundle="${msg}" />" height="46" />
			   		</c:if>
			   		<c:if test="${errorType == 'notFound'}">
   		            	<input type="image" src="imgs/btnLogin_${langInd}.png" name="image" width="<fmt:message key="btnsz.login" bundle="${msg}" />" height="46" />
   		            </c:if>
        		</form>	
			</div>
		</div>
	</div>
	<div id="footer-wrapper">
		<jsp:include page="footer_${langInd}.jsp" />
	</div>
</div>
</body>
</html>