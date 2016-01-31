/* write "size" bytes of "ptr" to "sd" */

writen(int sd, char *ptr, int size)
{
    int no_left, no_written;

    no_left = size;
    while (no_left > 0)
    {
       no_written = write(sd, ptr, no_left);
       if (no_written <=0)
            return(no_written);
       no_left -= no_written;
       ptr += no_written;
    }
    return(size - no_left);
}
