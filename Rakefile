# vim:ft=ruby:ts=2:sw=2:et:
begin
  require 'rubygems'
  require 'hoe'
  require 'vlad' 
  require 'vlad/local'
  require 'vlad/generate'  
  Vlad.load(:app => nil, :scm => nil, :config => 'vlad/deploy.rb')
rescue LoadError
  # do nothing
end

set :application, "dgd7"
set :db_user, application
set :db_pass, ""
set :db_file, "/etc/mysql/drupal.cnf"
set :web_user, "www-data"
set :web_group, "www-data"
set :origin, "origin"
set :branch, "master"
set :drupal_dir, "drupal"

set :test_domain, "simone.mayfirst.org"
set :test_deploy_to, "/var/local/drupal/#{application}"
set :test_release_path, "#{test_deploy_to}/#{drupal_dir}"
set :prod_domain, "definitivedrupal@sojourner.mayfirst.org"
set :prod_deploy_to, "/var/local/drupal/dgd7"
set :prod_release_path, "#{prod_deploy_to}/#{drupal_dir}"

task :local do
  set :db_name, application
end

task :test do
  set :environment, "test"
  set :repository, "git@github.com:definitivedrupal/#{application}.git"
  set :db_name, "drupal-#{application}"
  set :deploy_to, "/var/local/drupal/#{application}"
  set :release_path, test_release_path

  role :app, test_domain
  role :web, test_domain
  role :db, test_domain
  role :repo, test_domain
end

task :prod do
  set :environment, "production"
  set :repository, "git@github.com:definitivedrupal/#{application}.git"
  set :db_name, "drupal-#{application}"
  set :deploy_to, prod_deploy_to
  set :release_path, prod_release_path

  role :app, prod_domain
  role :web, prod_domain
  role :db, prod_domain
  role :agaric_live, prod_domain
end
