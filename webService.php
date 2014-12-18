<?php

/*
 * This php code was written to connect to a client's web service with supplied
 * parameters (url, member ID, and a few others), parse the response and send it
 * back to the original application in the form of xml.  The purpose of this
 * web service is to validate a customer against the client's backend before
 * allowing them to continue further in the application.  As this was code I
 * developed for my current employer, I have stripped the php of any and all code
 * pertaining specifically to the client, as well as information regarding my
 * employer and their back-end functionality.  I have placed comments throughout
 * this code for clarity.  
 * 
 * This code was developed by Chris Wright, 2014.
 */

   require_once('SoapClientTimeOut.inc');

   // Setting the location of the error log.
   ini_set("error_log", "/log_location/webservice.log");


  /* printResponse(respData)
   * Prints Response Data back to App as <Name>Value</Name> fields.
   * Input Data must be an Associative Array (Name => Value)
   */
   function printResponse(&$respData) {
      echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?";
      echo ">\n";
      echo "<response>\n";
      foreach ($respData as $key => $value) {
         echo "<$key>$value</$key>\n";
         error_log("Info: Response Element - <$key>$value</$key>");
      }
      echo "</response>\n";
   }

   /*
    * This class contains variables and information specific to the request header.
	* It is used to form the entire request to the client's web service.
	* Most of this code has been ommitted for security reasons.
	*/
   class SvcHeaderIn {
      public $ClientId;

      function __construct($clientId) {
         $this->ClientId = $clientId;
      }
   }

   /*
    * This class is also used in the formal request to the client's web service.
	* It contains member specific information that is used for validation on 
	* the client side.
	* Most of this code has been ommitted for security reasons.
	*/
   class ValidateMember_InData {
      public $MemberID;

      function __construct($memberId) {
               $this->MemberID = $memberId;
      }
   }

   /*
    * This class holds all of the information (Header and Data) for the web
	* service request.
	*/
   class ValidateMember_In {
      public $Header;
      public $Data;

      function __construct($memberId, $clientId) {
         $this->Header = new SvcHeaderIn($clientId);
         $this->Data = new ValidateMember_InData($memberId);
      }
   }

   class ValidateMember {
      public $input;

      function __construct($memberId, $clientId) {
         $this->input = new ValidateMember_In($memberId, $clientId);
      }
   }

   // Variables to hold the clientId and memberId fields
   $clientId = "";
   $memberId = "";

   // Here we're checking to make sure the memberId was supplied to the php. The WS
   // doesn't work without it. If it's missing, we log the error and return error xml
   // to the original application.
   if (isset($_REQUEST['memberId'])) {
      $memberId = $_REQUEST['memberId'];
   } else {
      error_log("Bad Input: Request failed with insufficient data. Missing memberId.");
      $respArr = array('StatusCode' => -1,
                      'StatusMessage' => 'Error: Member ID must be specified');
      printResponse($respArr);
      exit(1);
   }

   // The URLs to our customer's web services don't always remain the same, particularly
   // in situations where the client has separate test-environment URLs. Rather than
   // change URLs in multiple places, the URL is supplied to the php.
   if ( !isset($_REQUEST['url']) || empty($_REQUEST['url']) ) {
      $url_location = "http://clienturl.com/ValidateMemberWS.asmx?WSDL";
      error_log("Bad Input: Missing URL parameter. Using default [$url_location]");
   } else {
      $url_location = trim($_REQUEST['url']);
   }   

   // Finally, we check for the clientId parameter.  This is of lesser importance, so
   // I'm not returning any errors if this was not provided.
   if (isset($_REQUEST['clientId'])) {
      $clientId = $_REQUEST['clientId'];
   }

   $apiDown = false;
   $statusCode = 200;
   $statusMessage = "Success";
   $respArr = "";
   
   // Form the request data using the supplied parameters
   $reqData =  new ValidateMember($memberId, $clientId);

   try {
      $soapClient = new SoapClientTimeOut($url_location, array("trace" => 1,
                                                         "exceptions"=> 1));
														 
      // Set a timeout so that the web service doesn't sit and spin on error														 
      $soapClient->__setTimeout(15);
      try {
         $validationStatus = "";
		 
		 // Here, we're submitted the request and storing the response.
         $response = $soapClient->ValidateMember($reqData);
         $resText = print_r($soapClient->__getLastResponse(), true);

         // CLIENT SPECIFIC STUFF

		 // If response is empty, obviously something went wrong. Therefore, we return
		 // errors in the log and set the appropriate variables.
         if ( empty($response) || !isset($response) ) {
            $statusCode = 3;
            error_log("Error: No Response Received from Web Service.");
            $apiDown = true;
            $statusMessage = "Error: No Response Received from Web Service.";
         } else if ( empty($response->ValidateMemberResult->Header) ||
                     !isset($response->ValidateMemberResult->Header->ResultCode) ) {
            $statusCode = 5;
            $statusMessage = "Error: Failed to parse XML response data for ResultCode element.";
            $apiDown = true;
            error_log("Error: Failed to parse XML response data for ResultCode element.");
         } else if ( $response->ValidateMemberResult->Header->ResultCode != 0 ) {
            $statusCode = 5;
            $statusMessage = $response->ValidateMemberResult->Header->ResultDescription;
            error_log("Warn: ResultCode [".$response->ValidateMemberResult->Header->ResultCode. "] error: ".$statusMessage );
         } else {
		    // Otherwise, the response was good.
            if ( (isset($response->ValidateMemberResult->Data->ValidationStatus) ) &&
                (!empty($response->ValidateMemberResult->Data->ValidationStatus) ) ) {

               $validationStatus = $response->ValidateMemberResult->Data->ValidationStatus;
            }
         }

		 // Here, I'm creating the response array in key=>value format.
         $respArr = array('apiDown' => (($apiDown) ? "Y" : "N"),
                             'StatusCode' => $statusCode,
                             'StatusMessage' => $statusMessage,
                             'ValidationStatus' => $validationStatus );
							 
	  // Here's all the exception handing for Soap faults and unknown exceptions.
	  // I'm logging the appropriate messages in the error log and the response array.
      } catch (SoapFault $rfault) {
         if (stristr($rfault->__toString(), "[HTTP]") !== FALSE) {
            $statusCode = 2;

            // CLIENT SPECIFIC STUFF
         }
         $resText = print_r($soapClient->__getLastResponse(), true);

            // CLIENT SPECIFIC STUFF

         $respArr = array('apiDown' => "Y",
                          'StatusCode' => $statusCode,
                          'StatusMessage' => "Error: SOAP Fault." );
   } catch (Exception $e) {
            $resText = print_r($soapClient->__getLastResponse(), true);

            // CLIENT SPECIFIC STUFF

            $respArr = array('apiDown' => "Y",
                             'StatusCode' => 4,
                             'StatusMessage' => "ERROR: PHP Exception: " .$e->getMessage() );
   } catch (SoapFault $cfault) {

        // CLIENT SPECIFIC STUFF

        $respArr = array('apiDown' => "Y",
                         'StatusCode' => 2,
                         'StatusMessage' => "ERROR: Connection Error: " . $cfault->faultstring );
   } catch (Exception $e) {

      // CLIENT SPECIFIC STUFF

      $respArr = array('apiDown' => "Y",
                       'StatusCode' => 2,
                       'StatusMessage' => "ERROR: Connection Error: " .$e->getMessage() );
   }
   
   // Finally, I call printResponse(), which formats the response into
   // xml to send back to the original application.
   printResponse($respArr);
   return 0;
?>
