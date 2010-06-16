# vim:ft=ruby:ts=2:sw=2:et:
namespace :vlad do
  Rake.clear_tasks( "vlad:setup_app", 
                    "vlad:update", 
                    "vlad:start_web",
                    "vlad:start",
                    "vlad:cleanup",
                    "vlad:rollback"
                  )

  desc "Full deployment".cleanup
    
  task :deploy do
    Rake::Task["vlad:setup"].invoke
    Rake::Task["vlad:update"].invoke
    Rake::Task["vlad:update_settings"].invoke

    if environment == "prod"
      Rake::Task["vlad:update_vhost"].invoke
      Rake::Task["vlad:start"].invoke
    end

    puts "Successfully installed #{application} on #{environment} environment"
  end

  desc "Cleanup, drop database, drop database user tasks combined for convenience.".cleanup

  task :undeploy => %w[
    vlad:cleanup
    vlad:db:drop
    vlad:db:drop_user
  ]

  desc "Creates a database and a database user.".cleanup

  task :setup => %w[
    db:create_user
    db:create
  ]

  desc "Enables the website and its cronjob.".cleanup

  task :start => %w[
    vlad:enable_site
    vlad:start_web
    vlad:start_cron
  ]
  
  desc "Prepares application servers for deployment.".cleanup

  remote_task :setup_app, :roles => :app do
    files_dir = "#{release_path}/sites/default/files"
    database_dir = "#{deploy_to}/db"

    run "git clone -o agaric #{repository} #{deploy_to}"
    run "[ -d #{files_dir} ] || mkdir #{files_dir}"
    run "[ -d #{database_dir} ] || mkdir #{database_dir}"

    if environment == "test"
      sudo "chmod g+w #{deploy_to}"
      sudo "chown #{web_user}:staff #{files_dir}"
    end
  end

  desc "Creates a fastcgi wrapper.".cleanup
  
  remote_task :setup_fcgid, :roles => :app do
    fcgid_wrapper = "#{deploy_to}/php5-cgi"

    run "cp .. #{fcgid_wrapper}"
    sudo "chown #{application}:#{application} fcgid_wrapper"
    sudo "chmod 700 #{application}"
  end

  desc "Updates your application server to the latest revision.
    sets file ownership and permissions.".cleanup

  remote_task :update, :roles => :app do
    system "git push agaric master"
    run [ "cd #{deploy_to}",
          "git checkout master",
          "git pull agaric master",
        ].join(" && ")
  end

  desc "Reloads the server if the configuration passes the syntax 
    check.".cleanup

  remote_task :start_web, :roles => :agaric_live do
    sudo "apache2ctl -t"
    sudo "#{web_command} reload"
  end

  desc "Rolls back to the previous release.".cleanup

  remote_task :rollback, :roles => :app do
    run "cd #{deploy_to} && git checkout ORIG_HEAD"
  end

  desc "Creates a new bare repository in the web accessible git directory and syncs it with the local repository.".cleanup
  
  remote_task :setup_repo, :roles => :repo do
    repo_path = "/srv/git/#{application}.git"

    run [ "mkdir #{repo_path}",
          "cd #{repo_path}",
          "git init --bare",
          "chmod a+x #{repo_path}/hooks/post-update"
        ].join(" && ")
    
    system "git remote add agaric git.agaric.com:#{repo_path}"
    system "git push agaric master"
  end

  desc "Sets up a trac environment for the project".cleanup

  remote_task :setup_trac, :roles => :repo do
    trac_parent_dir = "/srv/trac"
    trac_env = "#{trac_parent_dir}/#{application}"
    trac_global_ini = "/etc/trac/trac.ini"

    sudo "trac-admin #{trac_env} initenv --inherit=#{trac_global_ini} #{application} sqlite:db/trac.db git #{repository}" 
    sudo "chown -R www-data:www-data #{trac_env}" 
    sudo "trac-admin #{trac_env} permission add ben TRAC_ADMIN" 
    sudo "trac-admin #{trac_env} permission add dan TRAC_ADMIN" 
    sudo "trac-admin #{trac_env} permission add stefan TRAC_ADMIN" 
    sudo "trac-admin #{trac_env} permission add tom TRAC_ADMIN" 
    sudo "trac-admin #{trac_env} permission add manda TRAC_ADMIN" 
  end
  
  desc "Updates the drupal settings for the selected environemnt.".cleanup
    
  remote_task :update_settings do
    source = "#{release_path}/sites/default/default.settings.php"
    target = "#{release_path}/sites/default/settings.php"

    unless uptodate?(target, source) 
      db_url = "mysql:\\/\\/#{db_user}:#{db_pass}@localhost\\/#{db_name}"
      run "sed 's/^$db_url.*/$db_url = \"#{db_url}\";/' #{source}>#{target}" 
    end
  end

  desc "Updates the vhost configuration.".cleanup

  remote_task :update_vhost, :roles => :agaric_live do
    vhost_path = "#{deploy_to}/vhost"
    sudo "cp #{vhost_path} #{web_sites}/#{application}"
  end	

  desc "Enables the virtual host configuration.".cleanup

  remote_task :enable_site, :roles => :agaric_live do
    sudo "a2ensite #{application}"
  end

  desc "Disables the virtual host configuration.".cleanup

  remote_task :disable_site, :roles => :agaric_live do
    sudo "a2dissite #{application}"
  end

  task :disable_site => :stop_cron

  desc "Starts the cronjob.".cleanup

  remote_task :start_cron, :roles => :agaric_live do
    file = File::open('vhost', 'r')
    cron_script = "nice -n 19 drush -r #{release_path} cron"
    cron_job = "#{rand 60} \\* \\* \\* \\* #{application} #{cron_script}"

    sudo "echo #{cron_job} | sudo tee -a /etc/cron.d/drupal"
  end

  desc "Stops the cronjob.".cleanup

  remote_task :stop_cron, :roles => :agaric_live do
  end

  desc "Commits database and files from the target server to the 
    repository.".cleanup

  remote_task :commit do
    msg = "Snapshot of database and files."
    run [ "cd #{deploy_to}",
          "git add .",
          "git commit -a -m \"#{msg}\"", 
          "git push agaric +master"
        ].join(" && ")
    system "git pull agaric master"
  end

  task :commit => "db:dump"

  desc "Removes the files created by the setup and update tasks.".cleanup

  remote_task :cleanup do
    run "rm -rf #{deploy_to}"
  end

  namespace :db do

    remote_task :create, :roles => :db do
      sudo "mysqladmin #{db_file} create #{db_name}"
    end

    remote_task :create_user, :roles => :db do
      privileges = "ALL"

      sudo "mysql #{db_file} -e \"GRANT #{privileges} ON \\`#{db_name}\\`.* TO '#{db_user}'@'localhost' IDENTIFIED BY '#{db_pass}'\""
    end
     
    desc "Drops the project's entire database.".cleanup

    remote_task :drop, :roles => :db do
      sudo "mysqladmin #{db_file} -f drop #{db_name}"
    end
      
    desc "Drops the project's database user.".cleanup

    remote_task :drop_user, :roles => :db do
      sudo "mysql #{db_file} -e \"DROP USER '#{db_user}'@'localhost'\""
      puts "User \"#{db_user}\" dropped"
    end

    desc "Dumps the database with drush.".cleanup

    remote_task :dump do
      run "cd #{release_path} && drush sql dump --structure-tables-key=common --result-file=../db/#{environment}.sql"
    end

    desc "Restores the project's database.".cleanup
       
    remote_task :restore do
      run "mysql #{db_name} < #{deploy_to}/db/#{environment}.sql"
    end

    task :restore => :backup

    desc "Restores any saved production database to the test environment. Only 
      runs for development.".cleanup

    remote_task :restore_production do
      run "mysql #{db_name} < #{deploy_to}/db/production.sql"
    end

    task :restore_production => "vlad:update"

    desc "Makes a backup of the project's database in /tmp.".cleanup
    
    remote_task :backup do
      time = Time.new.strftime("%Y%m%dT%H%M%S")
      run "mysqldump #{db_name} > /tmp/#{db_name}_#{time}.sql" 
    end

  end

end

