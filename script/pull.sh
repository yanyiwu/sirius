SQL="set character_set_client = utf8; set character_set_connection = utf8; set character_set_results = utf8;select id, title, content from ci_paper; "
mysql -h 10.109.246.13 -u root -p combintel -N -e "$SQL"
