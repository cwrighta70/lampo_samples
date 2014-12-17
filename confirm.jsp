<%@ page language="java" contentType="text/html; charset=ISO-8859-1"
    pageEncoding="ISO-8859-1"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
    <link rel="stylesheet" href="tuftsMedical.css" type="text/css" media="all" />
    <title>Confirmation</title>
</head>
<body>
    <div id="wrapper">
	    <div id="header" class="header">
	        <p class="headerText1"><span style="color:rgb(74,170,66)">Thank you</span> for confirming your appointment.</p>
	    </div>
	    <div id="header2" class="header2"></div><br/><br/>
	    <div id="content">
	        <p>Please be sure to bring your photo identification, insurance cards, any 
	           <br/>co-payments due, a managed care referral from your primary care physician,
	           and a current list of medications.</p>
	    
	        <p>If you have new insurance or any of your contact information has changes,
	           please call Registration prior to your appointment at ${clinicPhone}
	           <br/>Monday to Friday, 8 am to 4:30 pm.</p>
	    
	        <p>We are located on ${location} at ${address}.</p><br/>
	    </div>
	    <div id="more">
	        <p>LEARN MORE ABOUT:</p>
	        <img src="imgs/Directions_RESIZED.jpg"/>&nbsp;&nbsp;<a href="https://www.tuftsmedicalcenter.org/About-Us/Locations-and-Directions.aspx">Directions</a><br/>
	        <img src="imgs/Parking_RESIZED.jpg"/>&nbsp;&nbsp;<a href="https://www.tuftsmedicalcenter.org/About-Us/Locations-and-Directions/Campus-Buildings-Maps/Parking.aspx">Parking in our garage</a><br/>
	        <img src="imgs/Valet_RESIZED.jpg"/>&nbsp;&nbsp;<a href="https://www.tuftsmedicalcenter.org/About-Us/Locations-and-Directions/Campus-Buildings-Maps/Parking.aspx">Valet parking</a><br/>
	        <img src="imgs/Getting_RESIZED.jpg"/>&nbsp;&nbsp;<a href="https://www.tuftsmedicalcenter.org/About-Us/Locations-and-Directions/Campus-Buildings-Maps.aspx">Getting around</a><br/>
	        <img src="imgs/Amenities_RESIZED.jpg"/>&nbsp;&nbsp;<a href="https://www.tuftsmedicalcenter.org/patient-care-services/Visitor-Info/Places-to-Eat.aspx">Amenities</a><br/>
	    
	    </div>
	    <div id="footer-wrapper">
		    <jsp:include page="footer.jsp" />
	    </div>
	</div>
</body>
</html>