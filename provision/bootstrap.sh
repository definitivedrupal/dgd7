#!/usr/bin/env bash
export DEBIAN_FRONTEND=noninteractive

apt-get update
apt-get -y upgrade
apt-get -y install \
  rake \
  ruby-sass \
  ruby-compass

cp -r /vagrant/provision/etc/* /etc/

export DRAKE=/usr/local/share/drake
if [ ! -d $DRAKE ]; then
	git clone vlad@simone.mayfirst.org:/srv/git/agaric/drake.git $DRAKE
fi

chmod -R u+w /vagrant/web/sites/default
cp /vagrant/provision/settings.php /vagrant/web/sites/default/

export FILES=/var/local/drupal
if [ ! -d $FILES ]; then
	mkdir -p $FILES
	chown -R www-data:staff $FILES
	chmod -R g+w $FILES
fi

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
