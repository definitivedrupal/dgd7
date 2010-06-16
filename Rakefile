# vim:ft=ruby:ts=2:sw=2:et:
begin
  require 'rubygems'
  require 'hoe'
  require 'vlad' 
  require 'config/generate.rb'
  Vlad.load(:app => nil, :scm => nil)
rescue LoadError
  # do nothing
end

set :application, "dgd7"

task :test do
  set :environment, "test"
  set :domain, "simone.mayfirst.org"
  set :repository, "/srv/git/#{application}.git"
  set :web_command, "/etc/init.d/apache2"
  set :web_user, "www-data"
  set :web_group, "www-data"
  set :web_sites, "/etc/apache2/sites-available"
  set :db_file, "--defaults-file=/etc/mysql/drupal.cnf"
  set :db_name, "drupal-#{application}"
  set :db_user, "#{application}"
  set :db_pass, "example"
  set :deploy_to, "/var/local/drupal/#{application}"
  set :release_path, "#{deploy_to}/web"

  role :app, domain
  role :web, domain
  role :db, domain
  role :repo, domain
end

task :prod do
  set :environment, "production"
  set :domain, "definitivedrupal@sojourner.mayfirst.org"
  set :repository, "vlad@git.agaric.com:/srv/git/#{application}.git"
  set :web_command, "/etc/init.d/apache2"
  set :web_user, "www-data"
  set :web_group, "www-data"
  set :web_sites, "/etc/apache2/sites-available"
  set :db_file, "--defaults-file=/etc/mysql/drupal.cnf"
  set :db_name, "drupal-#{application}"
  set :db_user, "#{application}"
  set :db_pass, "example"
  set :deploy_to, "/var/local/drupal/#{application}"
  set :release_path, "#{deploy_to}/drupal"

  role :app, domain
  role :web, domain
  role :db, domain
  role :agaric_live, domain
end
