require 'date'
require 'rake/clean'

DEFAULT_SETTINGS = "web/sites/default/default.settings.php"

CLEAN.include("build")

ENVIRONMENTS.keys.each do |env|
  settings_source = "etc/#{env}.settings.php"
  settings_target = "build/#{env}/sites/default/settings.php"

  release_host = ENVIRONMENTS[env][0]
  release_path = ENVIRONMENTS[env][1]
  release_tag = ENVIRONMENTS[env][2]

  settings_env = "settings_#{env}".to_sym
  compass_env = "compass_#{env}".to_sym
  build_env = "build_#{env}".to_sym
  upload_env = "upload_#{env}".to_sym
  deploy_env = "deploy_#{env}".to_sym

  CLOBBER.include(settings_source)

  file "build/#{env}" do
    commit = release_tag ? release_tag : 'HEAD'
    mkdir_p "build/#{env}"
    sh "git archive #{commit} | tar -x -C build/#{env} --strip-components=1"
  end

  task compass_env do
    Dir.glob("build/#{env}/sites/*/themes/**/config.rb") do |project|
      sh "compass compile -e production --force #{File.dirname(project)}"
    end
  end

  directory "etc"

  task settings_env => "etc" do
    unless File.exists?(settings_source)
      cp DEFAULT_SETTINGS, settings_source
    end
  end

  file settings_target => settings_env do
    cp settings_source, settings_target
  end

  build_version = "build/#{env}/BUILD_VERSION.txt"

  file build_version do
    sh "git describe --always --long #{release_tag} > #{build_version}"
  end

  desc "Build the #{env} environment."
  task build_env => [:clean, "build/#{env}", settings_target, build_version]

  desc "Upload #{env} environment to the configured host."
  task upload_env => build_env do
    sh "ssh #{release_host} '([ -d #{release_path} ] || mkdir -p #{release_path})'"
    rsync_options = "-rz --stats --exclude default/files --exclude config.rb --exclude sass/ --exclude .sass-cache/ --delete"
    rsync_source = "build/#{env}/"
    rsync_target = "#{release_host}:#{release_path}"
    sh "rsync #{rsync_options} #{rsync_source} #{rsync_target}"
  end

  db_backup_task = "db_backup_#{env}".to_sym
  task db_backup_task do
    file = "~/backup-#{DateTime.now}.sql"
    sh "ssh #{release_host} 'drush -r #{release_path} sql-dump > #{file}'"
  end

  db_drop_tables_task = "db_drop_tables_#{env}".to_sym
  task db_drop_tables_task => db_backup_task do
    sh "ssh #{release_host} drush -y -r #{release_path} sql-drop"
  end

  desc "Deploy the #{env} environment to the configured host."
  task deploy_env => [db_backup_task, upload_env] do
    files_path = "#{release_path}/sites/default/files"
    commands = [
      "([ -d #{files_path} ] || mkdir #{files_path})",
      "drush -y -r #{release_path} updatedb",
      "drush -y -r #{release_path} cc all",
    ].join(" && ")
    sh "ssh #{release_host} '#{commands}'"
  end

  file_sync_task = "file_sync_#{env}_to_local".to_sym
  desc "Sync files from #{env} to local environment."
  task file_sync_task do
    sh "rsync -rz --stats --exclude styles --exclude css --exclude js --delete \
      #{release_host}:#{release_path}/sites/default/files/ \
      web/sites/default/files/"
  end

  db_sync_task = "db_sync_#{env}_to_local".to_sym
  desc "Sync database from #{env} to local environment."
  task db_sync_task do
    drupal_root = "#{Dir.getwd()}/web"
    sh "drush -y -r #{drupal_root} sql-drop"
    sh "ssh -C #{release_host} drush -r #{release_path} \
      sql-dump --structure-tables-key=common | drush -r #{drupal_root} sql-cli"
  end

  desc "Compile Sass to CSS in the local environment."
  task "compass_compile" do
    Dir.glob("web/sites/*/themes/**/config.rb") do |project|
      sh "compass compile #{File.dirname(project)}"
    end
  end

  ENVIRONMENTS.keys.each do |e|
    unless e == env then
      from_host = ENVIRONMENTS[e][0]
      from_path = ENVIRONMENTS[e][1]

      file_sync_task = "file_sync_#{e}_to_#{env}".to_sym
      desc "Sync files from #{e} to #{env} environment."
      task file_sync_task do
        sh "ssh -A #{from_host} rsync -rz --stats --exclude styles \
          --exclude css --exclude js #{from_path}/sites/default/files/ \
          --delete #{release_host}:#{release_path}/sites/default/files/"
      end

      db_sync_task = "db_sync_#{e}_to_#{env}".to_sym
      desc "Sync database from #{e} to #{env} environment."
      task db_sync_task => db_drop_tables_task do
        sh "ssh -C #{from_host} drush -r #{from_path} \
          sql-dump --structure-tables-key=common | \
          ssh -C #{release_host} drush -r #{release_path} sql-cli"
      end
    end
  end

  desc "Build all environments."
  task :default => build_env
end

TAGNAMES.each do |tagname|
  desc "Tag a commit with #{tagname}."
  task "tag_#{tagname}".to_sym do
    sh "git fetch --tags"
    num = `git tag`.scan(Regexp.new(tagname + "-")).size + 1
    sh "git tag -am '#{tagname.upcase} Release #{num}' #{tagname}-#{num}"
    sh "git tag -afm 'Current #{tagname.upcase} Release' #{tagname}"
    sh "git push origin :refs/tags/#{tagname}"
    sh "git push origin --tags"
  end
end

