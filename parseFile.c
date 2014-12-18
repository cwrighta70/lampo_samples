/*****************************************************************************
 * General Overview
 *
 * A large portion of my current job responsibilities requires the use of C
 * programming for parsing through client-provided files, formatting data,
 * and creating reports for clients.  We have even used C to output web pages,
 * although this was quite annoying compared to using Ruby, PHP, etc.
 *
 * This particular example is from a pre-parse application I wrote for an
 * appointment notification client (meaning we contact people in a list to
 * remind them of an upcoming appointment).  The function below is only a
 * small excerpt of the application, but a largely important one.
 *
 ****************************************************************************/


/*****************************************************************************
 * NAME:        parse_file
 *
 * DESCRIPTION: This function parses through an input file line-by-line and
 *              reformats the data into a new file based on the values
 *              we need for our application.  It also allows us to sort through
 *              records that may be violating a particular set of rules and
 *              identify duplicate records.  Of course, much of that
 *              functionality has been stripped out of this example due to
 *              privacy and security reasons, but this should give a general
 *              idea of my C programming experience.
 *
 * PARAMETERS:  None
 *
 * LIMITATIONS:
 *
 * RETURNS:     SUCCESS
 *****************************************************************************/
int parse_file (void)
{
   char line[MXLENGTH],
        clinic_phone1[11],
        clinic_phone2[11],
        patientPhone[11],
        clinic_loc[21],
        account_id[ID_LEN],
        mrn[13],
        emailName[26],
        emailDomain[26],
        email[52];

   FILE *in, *phoneout, *emailout;


   GL_ENTER (__func__);

   // Here, I'm opening the client's input file for reading.  For future reference,
   // Cr is a struct for defining a control record.  As seen here, it holds the
   // filename that was passed as an argument to the app.
   in = fopen (Cr.fname, "r");
   if (in == NULL) {
      genlog( LVL1,  GL_NAME, "ERROR: Unable to open job file [%s]", Cr.fname );
      GL_LEAVE( -1 );
   }

   // I set up 2 separate output files, one for phone reminders and one for email reminders
   memset (Cr.phoneOutfile, '\0', sizeof(Cr.phoneOutfile));
   strcpy (Cr.phoneOutfile, Cr.fname);
   strtok (Cr.phoneOutfile, ".");
   strcat (Cr.phoneOutfile, ".txt");
   phoneout = fopen (Cr.phoneOutfile, "w");
   if (phoneout == NULL) {
      genlog( LVL1,  GL_NAME, "ERROR: Unable to open output file [%s]",
            Cr.phoneOutfile );
      GL_LEAVE( -1 );
   }

   memset (Cr.emailOutfile, '\0', sizeof(Cr.emailOutfile));
   strcpy (Cr.emailOutfile, Cr.fname);
   strtok (Cr.emailOutfile, ".");
   strcat (Cr.emailOutfile, "_email.txt");
   emailout = fopen (Cr.emailOutfile, "w");
   if (emailout == NULL) {
      genlog( LVL1,  GL_NAME, "ERROR: Unable to open output file [%s]",
            Cr.emailOutfile );
      GL_LEAVE( -1 );
   }

   // Initialize the memory now to make later logic cleaner.
   // Reallocation memory to the same size shouldn't
   // result in error.
   if ( (Cr.patients.list = (PATIENT *)calloc(1,
                                  sizeof(PATIENT))) == NULL ){
      genlog (LVL1, GL_NAME, "Memory allocation failed. Exiting.");
      GL_LEAVE( -1 );
   }

   // Now I process the file line by line.
   while (feof (in) == 0) {
      Cr.tot_read++;

      memset(line, 0, sizeof(line));
      fgets (line, sizeof(line), in );
      strtok (line, "\n\r");

      if (*line == '\0') {
         continue;
      }

      memset (clinic_phone1, '\0', sizeof(clinic_phone1));
      memset (clinic_phone2, '\0', sizeof(clinic_phone2));
      memset (patientPhone, '\0', sizeof(patientPhone));
      memset (account_id, '\0', sizeof(account_id));
      memset (mrn, '\0', sizeof(mrn));
      memset (clinic_loc, '\0', sizeof(clinic_loc));
      memset (emailName, '\0', sizeof(emailName));
      memset (emailDomain, '\0', sizeof(emailDomain));
      memset (email, '\0', sizeof(email));
      
      genlog (LVL1, GL_NAME, "parsing line %d", Cr.tot_read);

      sprintf (clinic_phone1, "%3.3s%3.3s%4.4s", line+304, line+308, line+312);
      genlog (LVL1, GL_NAME, "clinic_phone1 = %s", clinic_phone1);
      if (clinic_phone1[0] > 47 && clinic_phone1[0] < 58)
         sprintf (clinic_phone2, "%s", clinic_phone1);
      else
         sprintf (clinic_phone2, "%s", Cr.telnum);

      genlog (LVL1, GL_NAME, "clinic_phone2 = %s", clinic_phone2);

      sprintf (mrn, "%12.12s", line+13);
      remove_space(mrn);
      
      sprintf (patientPhone, "%10.10s", line+414);

      sprintf (clinic_loc, "%20.20s", line+284);
      trim(clinic_loc);
      genlog (LVL1, GL_NAME, "clinic_loc = %s", clinic_loc);
      
      // At this point, I check the record against a specific set of rules
      // to verify that it is valid.
      if ( validAppointment( line, clinic_phone2 ) ) {
         // MRN + YYMMDD
         sprintf (account_id, "%s%2.2s%2.2s%2.2s", mrn, line+184,
            line+178, line+181);
         genlog (LVL1, GL_NAME, "account_id = %s", account_id);
      } else {
         // This is an invalid appointment so keep the line separate
         // Missing or invalid data so use strange name to make the
         // line unique.
         // MRN + Strange
         sprintf (account_id, "%.12s%.10s", mrn, strange_name());
         genlog (LVL1, GL_NAME, "Invalid: account_id = %s", account_id);
      }

      // Get the email name and domain, combine them into one fully qualified address
      genlog (LVL1, GL_NAME, "Getting email");
      sprintf(emailName, "%.25s", line+447);
      sprintf(emailDomain, "%.25s", line+472);

	  trim(emailName);
	  trim(emailDomain);
      if (strlen(emailName) > 0 && strlen(emailDomain) > 0) {
    	  sprintf(email, "%s@%s", emailName, emailDomain);
    	  genlog (LVL1, GL_NAME, "Email was %s", email);
      }

      addPatientAppointment( account_id, clinic_phone2, patientPhone, report_group, email, line );

      Cr.jobcnt++;

      if (Cr.jobcnt % MXLENGTH == 0)
         genlog (LVL1, GL_NAME, "Working %d", Cr.jobcnt);

   } // end of while

   genlog (LVL1, GL_NAME, "Total Lines Read: %d", Cr.tot_read);
   genlog (LVL1, GL_NAME, "Total Unique Patient Visit Dates %ld", Cr.patients.num);

   fclose (in);

   // Need to print out appointments now one line per mrn per day
   printAppointments(emailout, phoneout);

   fflush (emailout);
   fflush (phoneout);
   fclose (emailout);
   fclose (phoneout);

   freeList();
   GL_LEAVE (SUCCESS);
}
