# vim:ft=ruby:ts=2:sw=2:et:
desc "Generates an Apache virtual host configuration file".cleanup

task 'vhost' do |t|
  File::open(t.name, 'w') do |f|
    f << <<-EOD
<VirtualHost *:80>
\tServerAdmin webmaster@agaricdesign.com
\tServerName 
			      
\tCustomLog /var/log/apache2/access.log vhost_combined         

\tDocumentRoot /var/www/#{application}

\t<Directory />
\t\tOptions FollowSymlinks
\t\t AllowOverride None
\t</Directory>

\t<Directory /var/www/#{application}>
\t\tOptions Indexes FollowSymLinks
\t\tAllowOverride All
\t\tOrder allow,deny
\t\tallow from all
\t</Directory>

</VirtualHost>
EOD
  end
end

