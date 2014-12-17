/*****************************************************************************
 * NAME:        parse_file
 *
 * DESCRIPTION: This function will read the input file and update the ivb
 *              users.
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
        doctor_num[7],
        clinic_loc[21],
        account_id[ID_LEN],
        mrn[13],
        report_group[4],
        ttsFlag[4],
        emailName[26],
        emailDomain[26],
        email[52];

   FILE *in, *phoneout, *emailout;


   GL_ENTER (__func__);

   in = fopen (Cr.fname, "r");
   if (in == NULL) {
      genlog( LVL1,  GL_NAME, "ERROR: Unable to open job file [%s]", Cr.fname );
      GL_LEAVE( -1 );
   }

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
      // Could just call exit, but oh well
      freeList();
      GL_LEAVE( -1 );
   }
   Cr.docCount = 0;
   Cr.clinicCount = 0;

   if ( load_bcl(DOCNUMBCL) != SUCCESS ) {
      genlog (LVL1, GL_NAME, "Failed loading Doctor BCL. Exiting.");
      freeList();
      GL_LEAVE( -1 );
   }

   if ( load_bcl(CLINICBCL) != SUCCESS ) {
      genlog (LVL1, GL_NAME, "Failed loading Clinic BCL. Exiting.");
      freeList();
      GL_LEAVE( -1 );
   }

   if ( Cr.clinicCount > 1 ) {
      qsort( Cr.clinicPrompts, Cr.clinicCount, sizeof( PROMPT_BCL ),
                   (int (*)()) cmpPrompts );
   }

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
      memset (report_group, '\0', sizeof(report_group));
      memset (doctor_num, '\0', sizeof(doctor_num));
      memset (clinic_loc, '\0', sizeof(clinic_loc));
      memset (ttsFlag, '\0', sizeof(ttsFlag));
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

      // Grab the doctor and clinic codes, then double check that we have 
      // matching prompts in the BCL
      sprintf (doctor_num, "%6.6s", line+216);
      genlog (LVL1, GL_NAME, "doctor_num = %s", doctor_num);
      sprintf (clinic_loc, "%20.20s", line+284);
      trim(clinic_loc);
      genlog (LVL1, GL_NAME, "clinic_loc = %s", clinic_loc);
      
      if ( validAppointment( line, report_group, clinic_phone2 ) ) {
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

	  memset (Cr.clinicPrompt, '\0', sizeof(Cr.clinicPrompt));
      genlog (LVL1, GL_NAME, "Moving to check_bcl");
      if (check_bcl(DOCNUMBCL, doctor_num, line) == SUCCESS) {
    	  if (check_bcl(CLINICBCL, clinic_loc, line) == SUCCESS) {
    		  sprintf (ttsFlag, "N/N");
    		  sprintf (line, "%s%s", line, Cr.clinicPrompt); // Clinic was found in BCL, add new prompt to line
    	  } else {
    		  sprintf (ttsFlag, "N/Y");
    		  sprintf (line, "%s     ", line); // Clinic was not found in BCL, add spaces to line to account for field
    	  }
      } else {
    	  if (check_bcl(CLINICBCL, clinic_loc, line) == SUCCESS) {
    		  sprintf (ttsFlag, "Y/N");
    		  sprintf (line, "%s%s", line, Cr.clinicPrompt);
    	  } else {
    		  sprintf (ttsFlag, "Y/Y");
    		  sprintf (line, "%s     ", line);
    	  }
      }
	  genlog (LVL1, GL_NAME, "Setting ttsFlag to %s", ttsFlag);
      sprintf (line, "%s%s", line, ttsFlag);
      genlog (LVL1, GL_NAME, "New line with flag is [%s]", line);
      
      // Get the email name and domain, combine them into one, then add to the
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
