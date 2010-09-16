$Id$

Job Scheduler
=============

Simple API for scheduling tasks once at a predetermined time or periodically at
a fixed interval.


Usage
=====

Add a job.

  $job = array(
    'callback' => 'mymodule_unpublish_nodes',
    'type' => 'story',
    'id' => 12,
    'period' => 3600,
    'periodic' => TRUE,
  );
  job_scheduler()->set($job);

Work off a job

  function mymodule_unpublish_nodes($job) {
    // Do stuff.
  }

Remove a job.

  $job = array(
    'callback' => 'mymodule_unpublish_nodes',
    'type' => 'story',
    'id' => 12,
  );
  job_scheduler()->remove($job);


Drupal Queue integration
========================

Not supported in Drupal 7 at the moment.


Example
=======

See Feeds module.


Hidden settings
===============

Hidden settings are variables that you can define by adding them to the $conf
array in your settings.php file.

Name:        job_scheduler_class
Default:     'JobScheduler'
Description: The class to use for managing schedule.
