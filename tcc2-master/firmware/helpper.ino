/******************************************************************

    @autor Jose Augusto Gorck
    @since 11/04/18

*/

void getSensorInfo(char *pdata) {
  pdata[0] = (char)SOH;
  pdata[1] = (char)SENDOR_TYPE;
  pdata[4] = (char)EOT;
}

