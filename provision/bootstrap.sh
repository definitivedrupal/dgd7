#!/usr/bin/env bash
export DEBIAN_FRONTEND=noninteractive
cp -r /vagrant/provision/etc/apt/* /etc/apt/

apt-get update
apt-get -y upgrade
apt-get -y install \
	rake \
	ruby-sass \
	ruby-compass
apt-get -y autoremove

cp -r /vagrant/provision/etc/* /etc/
chmod -R u+w /vagrant/web/sites/default
cp /vagrant/provision/settings.php /vagrant/web/sites/default/

export FILES=/var/local/drupal
if [ ! -d $FILES ]; then
	mkdir -p $FILES
fi
chown -R www-data:staff $FILES
chmod -R g+ws $FILES

if [ ! -L /vagrant/web/sites/default/files ]; then
	ln -s $FILES /vagrant/web/sites/default/files
fi

if [ ! -d /var/lib/mysql/drupal ]; then
	mysqladmin -u root create drupal
fi

if [ -L /etc/apache2/sites-enabled/000-default ]; then
	a2dissite 000-default
fi

if [ ! -L /etc/apache2/sites-enabled/drupal ]; then
	a2ensite drupal
fi

service apache2 restart
